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
#include "xtools.h"
#include "xdecode.h"
class XDecodeTask :public XThread
{
public:
    /// <summary>
    /// �򿪽�����
    /// </summary>
    bool Open(AVCodecParameters* para);

    //������������
    void Do(AVPacket* pkt) override; 

    //�߳�������
    void Main() override;

    // �̰߳�ȫ�����ص�ǰ��Ҫ��Ⱦ��AVFrame�����û�з���nullptr
    // need_view_������Ⱦ
    // ���ؽ����Ҫ�� XFreeFrame �ͷ�
    AVFrame* GetFrame();
private:
    std::mutex mux_;
    XDecode decode_;
    XAVPacketList pkt_list_;
    AVFrame* frame_ = nullptr;//�����洢
    bool need_view_ = false;    //�Ƿ���Ҫ��Ⱦ��ÿֻ֡��Ⱦһ�Σ�ͨ��GetFrame
};

