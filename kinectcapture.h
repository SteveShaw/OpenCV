#ifndef KINECTCAPTURE_H
#define KINECTCAPTURE_H

#include <Kinect.h>
#include <memory>
#include <opencv2/opencv.hpp>
#include <QScopedPointer>
#include <QDir>


class KinectCapture
{
public:
    KinectCapture();
    ~KinectCapture();

    bool Initialize();

    WAITABLE_HANDLE _evt_frame_ready;

    IMultiSourceFrameReader* const Reader()
    {
        return _frame_reader;
    }

    IColorFrameReader* const ColorReader()
    {
        return _icolor;
    }

    IDepthFrameReader* const DepthReader()
    {
        return _idepth;
    }

    IBodyFrameReader* const BodyReader()
    {
        return _ibody;
    }

    bool ProcessArrivedFrame(IMultiSourceFrameArrivedEventArgs *args);

		void ProcessDepthFrame(UINT16 *pBuffer, USHORT nMinDepth, USHORT nMaxDepth);

		void CaptureDepthFrame(IMultiSourceFrame* sf, int frame_count);

		bool CaptureColorFrame(IMultiSourceFrame* sf, int frame_count);

		void SetSaveDirectory(const char* path);


//    void WriteVideo();

//    bool StartWriteVideo(const char* path);

//    void EndWriteVideo();

    //bool CopyColorFrameData(IColorFrame *frame);
private:


    IKinectSensor *_iks; //kinect sensor object
//    std::unique_ptr<IMultiSourceFrameReader> _frame_reader;
    IMultiSourceFrameReader *_frame_reader;

    IColorFrameReader* _icolor;
    IDepthFrameReader* _idepth;
    IBodyFrameReader* _ibody;

    enum Dimension
    {
        ColorImageWidth = 1920,
        ColorImageHeight = 1080,
        ColorImageDepth = 4,
        ColorImageSize = ColorImageWidth*ColorImageHeight*ColorImageDepth,
        DepthImageWidth = 512,
        DepthImageHeight = 424,
        DepthImageSize = DepthImageWidth*DepthImageHeight
    };

//    int RGBSize = width * height * 4 * sizeof(unsigned char);
//    Mat RGBMat(height, width, CV_8UC4);
    cv::Mat m_color_mat;
    cv::Mat m_depth_mat;
    QScopedPointer<cv::VideoWriter> m_color_video_writer;
    QScopedPointer<cv::VideoWriter> m_depth_video_writer;

    QString m_tm_fmt;
//    QString m_save_dir;
    QDir m_save_dir;

    int m_frame_count;

    int _color_image_size;

		QByteArray m_save_path;
		QByteArray m_image_data;



//    IMultiSourceFrameReader *_frame_reader;

};

#endif // KINECTCAPTURE_H
