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
    //��ʼ����Ⱦ
    auto view = XVideoView::Create();
    view->Init(para->para);


    XDecodeTask decode_task;
    if (!decode_task.Open(para->para))
    {
        LOGERROR("open decode failed!");
    }
    else
    {
        //�趨һ�¸�����
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

