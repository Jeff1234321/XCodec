//////////////////////////////////  @��Ȩ˵��  //////////////////////////////////////////////////
///						Jiedi(China nanjing)Ltd.                                    
/// @��Ȩ˵�� ����Ϳγ̰�Ȩ���Ĳܿ���ӵ�в��Ѿ���������Ȩ���˴��������Ϊѧϰ�ο���������Ŀ��ʹ�ã�
/// �γ����漰����������Դ���������������Ӧ����Ȩ
/// �γ�Դ�벻����ֱ��ת�ص������Ĳ��ͣ�������������ƽ̨�������������������߿γ̡�
/// �γ����漰����������Դ���������������Ӧ����Ȩ               
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////  Դ��˵��  //////////////////////////////////////////////////
/// ��Ŀ����      : FFmpeg 4.2 �ӻ���ʵս-��·H265���¼�ſ��� ʵѵ��
/// Contact       : xiacaojun@qq.com
///  ����   :				http://blog.csdn.net/jiedichina
///	��Ƶ�γ� : �����ƿ���	http://study.163.com/u/xiacaojun		
///			   ��Ѷ����		https://jiedi.ke.qq.com/				
///			   csdnѧԺ		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
///           51ctoѧԺ		https://edu.51cto.com/sd/d3b6d
///			   ���Ŀ���		http://www.laoxiaketang.com 
/// �����������ڴ���ҳ����  http://ffmpeg.club
/// �����������γ�qqȺ ��639014264����ͬѧ�������������� 
/// ΢�Ź��ں�: jiedi2007
/// ͷ����	 : xiacaojun
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// �γ̽���qqȺ 639014264  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <thread>
#include <iostream>
#include <mutex>
#include <list>
struct AVPacket;
struct AVCodecParameters;
struct AVRational;
struct AVFrame;

//��־���� DEBUG INFO ERROR FATAL
enum XLogLevel
{
    XLOG_TYPE_DEBUG,
    XLOG_TYPE_INFO,
    XLOG_TPYE_ERROR,
    XLOG_TYPE_FATAL
};
#define LOG_MIN_LEVEL XLOG_TYPE_DEBUG
#define XLOG(s,level) \
    if(level>=LOG_MIN_LEVEL) \
    std::cout<<level<<":"<<__FILE__<<":"<<__LINE__<<":\n"\
    <<s<<std::endl;
#define LOGDEBUG(s) XLOG(s,XLOG_TYPE_DEBUG)
#define LOGINFO(s) XLOG(s,XLOG_TYPE_INFO)
#define LOGERROR(s) XLOG(s,XLOG_TPYE_ERROR)
#define LOGFATAL(s) XLOG(s,XLOG_TYPE_FATAL)


void MSleep(unsigned int ms);

//��ȡ��ǰʱ��� ����
long long NowMs();

void XFreeFrame(AVFrame** frame);


class XThread
{
public:
    //�����߳�
    virtual void Start();

    //ֹͣ�̣߳������˳���־���ȴ��߳��˳���
    virtual void Stop();

    //ִ������ ��Ҫ����
    virtual void Do(AVPacket* pkt) {}

    //���ݵ���һ������������
    virtual void Next(AVPacket* pkt)
    {
        std::unique_lock<std::mutex> lock(m_);
        if (next_)
            next_->Do(pkt);

    }

    //������������һ���ڵ㣨�̰߳�ȫ��
    void set_next(XThread* xt)
    {
        std::unique_lock<std::mutex> lock(m_);
        next_ = xt;
    }
protected:
    
    //�߳���ں���
    virtual void Main() = 0;

    //��־�߳��˳�
    bool is_exit_ = false;
    
    //�߳�������
    int index_ = 0;


private:
    std::thread th_;
    std::mutex m_;
    XThread *next_ = nullptr;//��������һ���ڵ�

};
class XTools
{
};



//����Ƶ����
class XPara
{
public:
    AVCodecParameters* para = nullptr;  //����Ƶ����
    AVRational* time_base = nullptr;    //ʱ�����

    //��������
    static XPara* Create();
    ~XPara();
private:
    //˽���ǽ�ֹ����ջ�ж���
    XPara();
};


/// <summary>
/// �̰߳�ȫavpacket list
/// </summary>
class XAVPacketList
{
public:
    AVPacket* Pop();
    void Push(AVPacket* pkt);
private:
    std::list<AVPacket*> pkts_;
    int max_packets_ = 100;//����б���������������
    std::mutex mux_;
};