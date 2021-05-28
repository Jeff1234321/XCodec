//////////////////////////////////  @版权说明  //////////////////////////////////////////////////
///						Jiedi(China nanjing)Ltd.                                    
/// @版权说明 代码和课程版权有夏曹俊所拥有并已经申请著作权，此代码可用作为学习参考并可在项目中使用，
/// 课程中涉及到的其他开源软件，请遵守其相应的授权
/// 课程源码不可以直接转载到公开的博客，或者其他共享平台，不可以用以制作在线课程。
/// 课程中涉及到的其他开源软件，请遵守其相应的授权               
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////  源码说明  //////////////////////////////////////////////////
/// 项目名称      : FFmpeg 4.2 从基础实战-多路H265监控录放开发 实训课
/// Contact       : xiacaojun@qq.com
///  博客   :				http://blog.csdn.net/jiedichina
///	视频课程 : 网易云课堂	http://study.163.com/u/xiacaojun		
///			   腾讯课堂		https://jiedi.ke.qq.com/				
///			   csdn学院		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
///           51cto学院		https://edu.51cto.com/sd/d3b6d
///			   老夏课堂		http://www.laoxiaketang.com 
/// 更多资料请在此网页下载  http://ffmpeg.club
/// ！！！请加入课程qq群 【639014264】与同学交流和下载资料 
/// 微信公众号: jiedi2007
/// 头条号	 : xiacaojun
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// 课程交流qq群 639014264  //////////////////////////////////////
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

//日志级别 DEBUG INFO ERROR FATAL
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

//获取当前时间戳 毫秒
long long NowMs();

void XFreeFrame(AVFrame** frame);


class XThread
{
public:
    //启动线程
    virtual void Start();

    //停止线程（设置退出标志，等待线程退出）
    virtual void Stop();

    //执行任务 需要重载
    virtual void Do(AVPacket* pkt) {}

    //传递到下一个责任链函数
    virtual void Next(AVPacket* pkt)
    {
        std::unique_lock<std::mutex> lock(m_);
        if (next_)
            next_->Do(pkt);

    }

    //设置责任链下一个节点（线程安全）
    void set_next(XThread* xt)
    {
        std::unique_lock<std::mutex> lock(m_);
        next_ = xt;
    }
protected:
    
    //线程入口函数
    virtual void Main() = 0;

    //标志线程退出
    bool is_exit_ = false;
    
    //线程索引号
    int index_ = 0;


private:
    std::thread th_;
    std::mutex m_;
    XThread *next_ = nullptr;//责任链下一个节点

};
class XTools
{
};



//音视频参数
class XPara
{
public:
    AVCodecParameters* para = nullptr;  //音视频参数
    AVRational* time_base = nullptr;    //时间基数

    //创建对象
    static XPara* Create();
    ~XPara();
private:
    //私有是禁止创建栈中对象
    XPara();
};


/// <summary>
/// 线程安全avpacket list
/// </summary>
class XAVPacketList
{
public:
    AVPacket* Pop();
    void Push(AVPacket* pkt);
private:
    std::list<AVPacket*> pkts_;
    int max_packets_ = 100;//最大列表数量，超出清理
    std::mutex mux_;
};