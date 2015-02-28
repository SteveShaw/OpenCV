#ifndef KINECTCAPTURE_H
#define KINECTCAPTURE_H

#include <Kinect.h>
#include <memory>
#include <opencv2/opencv.hpp>
#include <QScopedPointer>
#include <QDir>
#include <QTime>
#include <QxtCore/QxtBasicFileLoggerEngine>
#include <QxtCore/QxtLogger>

//#include "QVideoEncoder.h"
#include "dbitem.h"


class KinectCapture
{
public:
    KinectCapture();
    ~KinectCapture();

    bool Initialize();

    WAITABLE_HANDLE _evt_frame_ready;

    IMultiSourceFrameReader* const Reader()
    {
				return m_pFrameReader;
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




		void SetSaveDirectory(const char* path);

		void SetFrameCount(int num_count)
		{
			m_max_frame_count = num_count;
		}

		QString DirPath()
		{
			return QString(m_save_dir.path());
		}

		const char* FilePath(quint8 file_type)
		{
			switch(file_type)
			{
			case 0:
				return m_clr_file_path.toStdString().c_str();
			case 1:
				return m_dep_file_path.toStdString().c_str();
			default:
				return NULL;
			}
		}

		int CurrentFrameCount() const
		{
			return m_frame_count;
		}

		DBItem* GetDBItem()
		{
			return &m_db_item;
		}

		void Release();

protected:
		bool ProcessDepthFrame(IDepthFrame* pDepthFrame);

		bool ProcessColorFrame(IColorFrame* pColorFrame);

		void ProcessDepthFrame(UINT16 *pBuffer, USHORT nMinDepth, USHORT nMaxDepth);

//		void CaptureDepthFrame(IMultiSourceFrame* sf, int frame_count);

//		bool CaptureColorFrame(IMultiSourceFrame* sf, int frame_count);

		bool SaveAcquiredFrames(IColorFrame* cf, IDepthFrame* df);


//    void WriteVideo();

//    bool StartWriteVideo(const char* path);

//    void EndWriteVideo();

    //bool CopyColorFrameData(IColorFrame *frame);
private:


		IKinectSensor *m_pKinectSensor; //kinect sensor object
//    std::unique_ptr<IMultiSourceFrameReader> _frame_reader;
		IMultiSourceFrameReader *m_pFrameReader;

    IColorFrameReader* _icolor;
    IDepthFrameReader* _idepth;
    IBodyFrameReader* _ibody;

		ICoordinateMapper*	m_pCoordinateMapper;

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
		cv::Mat m_scaled_mat;
		cv::Mat m_bgr_mat;

		ColorSpacePoint* m_pColorSpacePoints;


    QScopedPointer<cv::VideoWriter> m_color_video_writer;
    QScopedPointer<cv::VideoWriter> m_depth_video_writer;

    QString m_tm_fmt;
//    QString m_save_dir;
    QDir m_save_dir;

		int m_frame_count; //current frame count

		int m_max_frame_count;

    int _color_image_size;

		QString m_clr_file_path;	//color file path
		QString m_dep_file_path; //depth file path
//		QByteArray m_image_data;



		//QVideoEncoder m_encoder;

		DBItem m_db_item;

		QTime m_Last; //to avoid record small video file in one minute



//    IMultiSourceFrameReader *_frame_reader;

};

#endif // KINECTCAPTURE_H
