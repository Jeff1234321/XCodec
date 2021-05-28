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

#include "xmux.h"

#include <iostream>
#include <thread>
using namespace std;
extern "C" { //ָ��������c���Ժ��������������������ر�ע
//����ffmpegͷ�ļ�
#include <libavformat/avformat.h>
}
void PrintErr(int err);

#define BERR(err) if(err!= 0){PrintErr(err);return 0;}

//////////////////////////////////////////////////
//// �򿪷�װ
AVFormatContext* XMux::Open(const char* url)
{
    AVFormatContext* c = nullptr;
    //����������
    auto re = avformat_alloc_output_context2(&c, NULL, NULL, url);
    BERR(re);

    //�����Ƶ��Ƶ��
    auto vs = avformat_new_stream(c, NULL);   //��Ƶ��
    vs->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    auto as = avformat_new_stream(c, NULL);   //��Ƶ��
    as->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;


    //��IO
    re = avio_open(&c->pb, url, AVIO_FLAG_WRITE);
    BERR(re);
    return c;
}
bool XMux::Write(AVPacket* pkt)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    //д��һ֡���ݣ��ڲ���������dts��ͨ��pkt=null ����д�뻺��
    auto re = av_interleaved_write_frame(c_,pkt);
    BERR(re);
    return true;
}

bool XMux::WriteEnd()
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    int re = 0;
    //auto re = av_interleaved_write_frame(c_, nullptr);//д�����򻺳�
    //BERR(re);
    re = av_write_trailer(c_);
    BERR(re);
    return true;
}
bool XMux::WriteHead()
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    //��ı�timebase
    auto re = avformat_write_header(c_, nullptr);
    BERR(re);

    //��ӡ���������
    av_dump_format(c_, 0, c_->url, 1);

    return true;
}