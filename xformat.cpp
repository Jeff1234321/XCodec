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

#include "xformat.h"
#include <iostream>
#include <thread>
#include "xtools.h"
using namespace std;
extern "C" { //ָ��������c���Ժ��������������������ر�ע
//����ffmpegͷ�ļ�
#include <libavformat/avformat.h>
}
//Ԥ����ָ����
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
using namespace std;

static int TimeoutCallback(void* para)
{
    auto xf = (XFormat*)para;
    if (xf->IsTimeout())return 1;//��ʱ�˳�Read
    //cout << "TimeoutCallback" << endl;
    return 0; //��������
}

void XFormat::set_c(AVFormatContext* c)
{
    unique_lock<mutex> lock(mux_);
    if (c_) //����ԭֵ
    {
        if (c_->oformat) //���������
        {
            if (c_->pb)
                avio_closep(&c_->pb);
            avformat_free_context(c_);
        }
        else if (c_->iformat)  //����������
        {
            avformat_close_input(&c_);
        }
        else
        {
            avformat_free_context(c_);
        }
    }
    c_ = c;
    if (!c_)
    {
        is_connected_ = false;
        return;
    }
    is_connected_ = true;

    //��ʱ ���ڳ�ʱ�ж�
    last_time_ = NowMs();

    //�趨��ʱ����ص�
    if (time_out_ms_ > 0)
    {
        AVIOInterruptCB cb = { TimeoutCallback ,this };
        c_->interrupt_callback = cb;
    }

    //���������Ƿ�����Ƶ������Ƶ��
    audio_index_ = -1; 
    video_index_ = -1;
    //��������Ƶstream ����
    for (int i = 0; i < c->nb_streams; i++)
    {
        //��Ƶ
        if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_index_ = i;
            audio_time_base_.den = c->streams[i]->time_base.den;
            audio_time_base_.num = c->streams[i]->time_base.num;
        }
        else if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index_ = i;
            video_time_base_.den = c->streams[i]->time_base.den;
            video_time_base_.num = c->streams[i]->time_base.num;
            video_codec_id_ = c->streams[i]->codecpar->codec_id;
        }
    }
}
std::shared_ptr<XPara> XFormat::CopyVideoPara()
{
    int index = video_index();
    shared_ptr<XPara> re;
    unique_lock<mutex> lock(mux_);
    if (index < 0 || !c_)return re;

    re.reset(XPara::Create());
    *re->time_base = c_->streams[index]->time_base;
    avcodec_parameters_copy(re->para, c_->streams[index]->codecpar);
    return re;
}
bool XFormat::CopyPara(int stream_index, AVCodecContext* dts)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)
    {
        return false;
    }
    if (stream_index<0 || stream_index>c_->nb_streams)
        return false;
    auto re = avcodec_parameters_to_context(dts, c_->streams[stream_index]->codecpar);
    if (re < 0)
    {
        return false;
    }
    return true;
}
/// <summary>
/// ���Ʋ��� �̰߳�ȫ
/// </summary>
/// <param name="stream_index">��Ӧc_->streams �±�</param>
/// <param name="dst">�������</param>
/// <returns>�Ƿ�ɹ�</returns>
bool XFormat::CopyPara(int stream_index, AVCodecParameters* dst)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)
    {
        return false;
    }
    if (stream_index<0 || stream_index>c_->nb_streams)
        return false;
    auto re = avcodec_parameters_copy(dst, c_->streams[stream_index]->codecpar);
    if (re < 0)
    {
        return false;
    }


    return true;
}

bool XFormat::RescaleTime(AVPacket* pkt, long long offset_pts, XRational time_base)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;        
    auto out_stream = c_->streams[pkt->stream_index];
    AVRational in_time_base;
    in_time_base.num = time_base.num;
    in_time_base.den = time_base.den;
    pkt->pts = av_rescale_q_rnd(pkt->pts- offset_pts, in_time_base,
            out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
        );
    pkt->dts = av_rescale_q_rnd(pkt->dts- offset_pts, in_time_base,
        out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
    );
    pkt->duration = av_rescale_q(pkt->duration, in_time_base, out_stream->time_base);
    pkt->pos = -1;

    return true;
}



//�趨��ʱʱ��
void XFormat::set_time_out_ms(int ms)
{
    unique_lock<mutex> lock(mux_);
    this->time_out_ms_ = ms;
    //���ûص�����������ʱ�˳�
    if (c_)
    {
        AVIOInterruptCB cb = { TimeoutCallback ,this };
        c_->interrupt_callback = cb;
    }
}