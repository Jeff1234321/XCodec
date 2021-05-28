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

#include <iostream>
#include <thread>
#include "xtools.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
using namespace std;
//class TestThread :public XThread
//{
//public:
//    void Main()
//    {
//        LOGDEBUG("TestThread Main begin");
//        while (!is_exit_)
//        {
//            this_thread::sleep_for(1ms);
//        }
//        LOGDEBUG("TestThread Main end");
//    }
//};
#define CAM1 \
"rtsp://admin:admin@192.168.2.108/cam/realmonitor?channel=1&subtype=0"
int main(int argc, char* argv[])
{
    XDemuxTask demux_task;
    for (;;)
    {
        if (demux_task.Open(CAM1))
        {
            break;
        }
        MSleep(100);
        continue;
    }

    auto para = demux_task.CopyVideoPara();
    //初始化渲染
    auto view = XVideoView::Create();
    view->Init(para->para);


    XDecodeTask decode_task;
    if (!decode_task.Open(para->para))
    {
        LOGERROR("open decode failed!");
    }
    else
    {
        //设定一下个责任
        demux_task.set_next(&decode_task);
        demux_task.Start();
        decode_task.Start();
    }

    for (;;)
    {
        auto f = decode_task.GetFrame();
        if (!f)
        {
            MSleep(1);
            continue;
        }
        view->DrawFrame(f);
        XFreeFrame(&f);
    }

    //TestThread tt;
    //tt.Start();
    //this_thread::sleep_for(3s);
    //tt.Stop();
    getchar();
    return 0;
}

