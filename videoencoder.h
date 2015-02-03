#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include "ffmpeg.h"

#include <QString>

class VideoEncoder
{
public:
	VideoEncoder();
	virtual ~VideoEncoder();

protected:
	 unsigned Width,Height;
	 unsigned Bitrate;
	 unsigned Gop;
	 bool ok;

	 // FFmpeg stuff
	 ffmpeg::AVFormatContext *pFormatCtx;
	 ffmpeg::AVOutputFormat *pOutputFormat;
	 ffmpeg::AVCodecContext *pCodecCtx;
	 ffmpeg::AVStream *pVideoStream;
	 ffmpeg::AVCodec *pCodec;
	 // Frame data
	 ffmpeg::AVFrame *ppicture;
	 uint8_t *picture_buf;
	 // Compressed data
	 int outbuf_size;
	 uint8_t* outbuf;
	 // Conversion
	 ffmpeg::SwsContext *img_convert_ctx;
	 // Packet
	 ffmpeg::AVPacket pkt;

	 QString fileName;

	 unsigned getWidth();
	 unsigned getHeight();
	 bool isSizeValid();

	 void initVars();
	 bool initCodec();

	 // Alloc/free the output buffer
	 bool initOutputBuf();
	 void freeOutputBuf();

	 // Alloc/free a frame
	 bool initFrame();
	 void freeFrame();

	 // Frame conversion
	 bool convertImage(const QImage &img);
	 bool convertImage_sws(const QImage &img);

	 virtual int encodeImage_p(const QImage &,bool custompts=false,unsigned pts=0);

private:
	 QString m_err_msg;

public:

	 bool CreateFile(QString filename,unsigned width,unsigned height,unsigned bitrate,unsigned gop,unsigned fps=25);
	 virtual bool close();

	 virtual int encodeImage(const QImage &);
	 virtual int encodeImagePts(const QImage &,unsigned pts);
	 virtual bool isOk();
};

#endif // VIDEOENCODER_H
