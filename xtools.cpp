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

#include "xtools.h"
#include <sstream>
using namespace std;
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
void XFreeFrame(AVFrame** frame)
{
    if (!frame || !(*frame))return;
    av_frame_free(frame);
}
void MSleep(unsigned int ms)
{
    auto beg = clock();
    for (int i = 0; i < ms; i++)
    {
        this_thread::sleep_for(1ms);
        if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
            break;
    }
}
long long NowMs()
{
    return clock() / (CLOCKS_PER_SEC / 1000);
}

//启动线程
void XThread::Start()
{
    unique_lock<mutex> lock(m_);
    static int i = 0;
    i++;
    index_ = i;
    is_exit_ = false;
    //启动线程
    th_ = thread(&XThread::Main, this);
    stringstream ss;
    ss << "XThread::Start()" << index_;
    LOGINFO(ss.str());
}

//停止线程（设置退出标志，等待线程退出）
void XThread::Stop()
{
    stringstream ss;
    ss << "XThread::Stop() begin" << index_;
    LOGINFO(ss.str());
    is_exit_ = true;
    if (th_.joinable()) //判断子线程是否可以等待
        th_.join();     //等待子线程退出
    ss.str("");
    ss << "XThread::Stop() end" << index_;
    LOGINFO(ss.str());
}


//创建对象
XPara* XPara::Create()
{
    return new XPara();
}
XPara::~XPara()
{
    if (para)
    {
        avcodec_parameters_free(&para);
    }
    if (time_base)
    {
        delete time_base;
        time_base = nullptr;
    }
}

//私有是禁止创建栈中对象
XPara::XPara()
{
    para = avcodec_parameters_alloc();
    time_base = new AVRational();
}

AVPacket* XAVPacketList::Pop()
{
    unique_lock<mutex> lock(mux_);
    if (pkts_.empty())return nullptr;
    auto pkt = pkts_.front();
    pkts_.pop_front();
    return pkt;
}

void XAVPacketList::Push(AVPacket* pkt)
{
    unique_lock<mutex> lock(mux_);
    //生成新的AVPacket 对象 引用计数+1;
    auto p = av_packet_alloc();
    av_packet_ref(p, pkt);//引用计数 减少数据复制，线程安全
    pkts_.push_back(p);

    //超出最大空间，清理数据，到关键帧位置
    if (pkts_.size() > max_packets_)
    {
        //处理第一帧
        if (pkts_.front()->flags & AV_PKT_FLAG_KEY)//关键帧
        {
            av_packet_free(&pkts_.front());//清理
            pkts_.pop_front();  //出队
            return;
        }
        //清理所有非关键帧之前的数据
        while (!pkts_.empty())
        {
            if (pkts_.front()->flags & AV_PKT_FLAG_KEY)//关键帧
            {
                return;
            }
            av_packet_free(&pkts_.front());//清理
            pkts_.pop_front();  //出队
        }
    }

}