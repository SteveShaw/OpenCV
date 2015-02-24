#include "kinectcapture.h"
#include <QDebug>
#include <QDateTime>

#define FRAMECOUNT 30

KinectCapture::KinectCapture()
	:m_pKinectSensor(NULL)
	,m_pFrameReader(NULL)
	,m_color_mat(KinectCapture::ColorImageHeight,KinectCapture::ColorImageWidth,CV_8UC4)
	,m_depth_mat(KinectCapture::DepthImageHeight,KinectCapture::DepthImageWidth,CV_8UC3)
	//,_color_image_size(KinectCapture::Width*KinectCapture::Height*KinectCapture::Depth)
	,m_tm_fmt("yyyyMMdd_HHmmss_zzz")
	,m_save_dir("E:/Video")
	,m_color_video_writer(new cv::VideoWriter)
	,m_depth_video_writer(new cv::VideoWriter)
	,m_frame_count(0)
	,m_max_frame_count(FRAMECOUNT)
	,m_pColorSpacePoints(NULL)
{
	//qDebug()<<"Color Image Size="<<ColorImageSize;
}


KinectCapture::~KinectCapture()
{
	qDebug()<<"release";

}

//Initialize Kinect Sensor
bool KinectCapture::Initialize()
{
	//    IKinectSensor *sensor = nullptr;
	if(FAILED(GetDefaultKinectSensor(&m_pKinectSensor)))
	{
		qDebug()<<"Fatal Error: Canno Find Default Kinect Sensor";
		return false;
	}

	//    _iks.reset(sensor);


	if(FAILED(m_pKinectSensor->Open()))
	{
		qDebug()<<"Fatal Error: Cannot Open Kinect Sensor";
		return false;
	}

	//To Initialize FFMPEG library
	m_color_video_writer->open("test1.m4v",CV_FOURCC('m','p','4','v'),8.0, cv::Size(640,360));
	m_depth_video_writer->open("test2.m4v",CV_FOURCC('m','p','4','v'),8.0, cv::Size(640,360));
	m_color_video_writer->release();
	m_depth_video_writer->release();

	DWORD frame_type = FrameSourceTypes::FrameSourceTypes_Depth |  FrameSourceTypes::FrameSourceTypes_Color;
	HRESULT hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);

	m_pColorSpacePoints	= new ColorSpacePoint[DepthImageWidth * DepthImageHeight];

	if (SUCCEEDED(hr))
	{
		if(FAILED(m_pKinectSensor->OpenMultiSourceFrameReader(frame_type,&m_pFrameReader)))
		{
			qDebug()<<"Fatal Error: Cannot Open Frame Reader";
			return false;
		}

		m_pFrameReader->SubscribeMultiSourceFrameArrived(&_evt_frame_ready);

		return true;
	}

	return false;
}

//bool KinectCapture::ProcessArrivedFrame(IMultiSourceFrameArrivedEventArgs *args)
//{
//	//    qDebug()<<"Process Arrived Frame Data";

//	IMultiSourceFrameReference *fr = NULL;

//	bool result = false;

//	if(SUCCEEDED(args->get_FrameReference(&fr)))
//	{
//		IMultiSourceFrame *sf = NULL;
//		if(fr && SUCCEEDED(fr->AcquireFrame(&sf)))
//		{
//			//CaptureColorFrame(sf);
//			if(m_frame_count==0)
//			{
//				m_db_item.start = QDateTime::currentDateTime().toString(m_tm_fmt);
//				m_db_item.dir = m_save_dir.path();
//				m_db_item.colorFileName = "Color_"+m_db_item.start+".m4v";
//				QString path = m_save_dir.absoluteFilePath(m_db_item.colorFileName);
//				m_color_video_writer->open(path.toStdString().c_str(),CV_FOURCC('m','p','4','v'),8.0, cv::Size(640,360));
//				m_db_item.depthFileName = "Depth_"+m_db_item.start+".m4v";
//				path = m_save_dir.absoluteFilePath(m_db_item.depthFileName);
//				m_depth_video_writer->open(path.toStdString().c_str(),CV_FOURCC('m','p','4','v'),8.0,
//																	 cv::Size(DepthImageWidth,DepthImageHeight));
//			}

//			CaptureColorFrame(sf,m_frame_count);
//			CaptureDepthFrame(sf,m_frame_count);
//			++m_frame_count;
//			//qDebug()<<"Count="<<m_frame_count;

//			if(m_frame_count==m_max_frame_count)
//			{
//				m_db_item.end = QDateTime::currentDateTime().toString(m_tm_fmt);
//				m_frame_count = 0;
//				//m_encoder.close();

//				if(m_depth_video_writer->isOpened())
//				{
//					m_depth_video_writer->release();
//				}

//				if(m_color_video_writer->isOpened())
//				{
//					m_color_video_writer->release();
//				}
//			}
//			sf->Release();
//			result = true;
//		}

//		fr->Release();
//	}

//	return result;

//}

bool KinectCapture::ProcessArrivedFrame(IMultiSourceFrameArrivedEventArgs *args)
{
	//    qDebug()<<"Process Arrived Frame Data";

	IMultiSourceFrameReference *pMultiSourceFrameReference = NULL;
	IMultiSourceFrame *pMultiSourceFrame = NULL;
	bool result = false;

	IColorFrame *pColorFrame = NULL;
	IDepthFrame *pDepthFrame = NULL;

	if(SUCCEEDED(args->get_FrameReference(&pMultiSourceFrameReference)))
	{

		if(SUCCEEDED(pMultiSourceFrameReference->AcquireFrame(&pMultiSourceFrame)))
		{

			//Capture Depth Frame
			IDepthFrameReference* pDepthFrameReference = NULL;
			HRESULT hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
			if (SUCCEEDED(hr))
			{
				hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);

				if(pDepthFrameReference!=NULL)
				{
					pDepthFrameReference->Release();
					pDepthFrameReference = NULL;
				}
			}

			//capture color frame

			if(SUCCEEDED(hr))
			{
				IColorFrameReference* pColorFrameReference = NULL;
				hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
				if(SUCCEEDED(hr))
				{
					hr = pColorFrameReference->AcquireFrame(&pColorFrame);
				}
				if(pColorFrameReference!=NULL)
				{
					pColorFrameReference->Release();
					pColorFrameReference = NULL;
				}
			}

			if(SUCCEEDED(hr))
			{
				result = this->SaveAcquiredFrames(pColorFrame,pDepthFrame);
			}

		}
	}

	if(pColorFrame!=NULL)
	{
		pColorFrame->Release();
		pColorFrame = NULL;
	}

	if(pDepthFrame!=NULL)
	{
		pDepthFrame->Release();
		pDepthFrame = NULL;
	}

	if(pMultiSourceFrame!=NULL)
	{
		pMultiSourceFrame->Release();
		pMultiSourceFrame = NULL;
	}

	if(pMultiSourceFrameReference!=NULL)
	{
		pMultiSourceFrameReference->Release();
		pMultiSourceFrameReference = NULL;
	}

	return result;

}

void KinectCapture::ProcessDepthFrame(UINT16 *pBuffer, USHORT nMinDepth, USHORT nMaxDepth)
{
	if (pBuffer!=NULL)
	{
		//        RGBQUAD* pRGBX = m_pDepthRGBX;

		// end pixel is start + width*height - 1
		//const UINT16* pBufferEnd = pBuffer + (DepthImageWidth * DepthImageHeight);

		int idx = 0;
		//int length = DepthImageSize;

		unsigned char* mat_data = m_depth_mat.ptr<unsigned char>();


		while (idx < DepthImageSize)
		{
			USHORT depth = pBuffer[idx];

			// To convert to a byte, we're discarding the most-significant
			// rather than least-significant bits.
			// We're preserving detail, although the intensity will "wrap."
			// Values outside the reliable depth range are mapped to 0 (black).

			// Note: Using conditionals in this loop could degrade performance.
			// Consider using a lookup table instead when writing production code.
			//depth = (depth-nMinDepth)/(nMaxDepth - nMinDepth)*256;
			BYTE intensity = 0;
			if(depth<nMaxDepth && depth>nMinDepth)
			{
				intensity = (BYTE)((double)(depth-nMinDepth)/(double)(nMaxDepth-nMinDepth)*256.0);
			}

			mat_data[idx*3] = intensity;
			mat_data[idx*3+1] = intensity;
			mat_data[idx*3+2] = intensity;

			++idx;
		}

	}
}

//void KinectCapture::CaptureDepthFrame(IMultiSourceFrame *sf, int frame_count)
//{
//	if(sf==NULL)
//	{
//		//		qDebug()<<"SF is NULL";
//		return;
//	}

//	IDepthFrameReference *idfr = NULL;

//	if(SUCCEEDED(sf->get_DepthFrameReference(&idfr)))
//	{
//		IDepthFrame* idf = NULL;

//		if(SUCCEEDED(idfr->AcquireFrame(&idf)))
//		{
//			USHORT min_dist;
//			USHORT max_dist;

//			if(SUCCEEDED(idf->get_DepthMinReliableDistance(&min_dist))
//				 &&SUCCEEDED(idf->get_DepthMaxReliableDistance(&max_dist)))
//			{
//				UINT buffer_size = 0;
//				UINT16 *ptr_buffer = NULL;

//				if(SUCCEEDED(idf->AccessUnderlyingBuffer(&buffer_size,&ptr_buffer)))
//				{
//					ProcessDepthFrame(ptr_buffer,min_dist,max_dist);

//					//					if(frame_count==0)
//					//					{

//					////						m_db_item.depthFileName = "Depth_"+m_db_item.start+".m4v";
//					////						QString path = m_save_dir.absoluteFilePath(m_db_item.depthFileName);
//					//						qDebug()<<"Start recording "+m_db_item.depthFileName;
//					////						m_depth_video_writer->open(path.toStdString().c_str(),
//					////																			 CV_FOURCC('m','p','4','v')
//					////																			 ,8.0,
//					////																			 //cv::Size(704,576));
//					////																			 cv::Size(DepthImageWidth,DepthImageHeight));

//					//						//_cvw->write(_color_image);
//					//					}


//					//					m_depth_video_writer->write(m_depth_mat);
//				}

//				if(idfr!=NULL)
//				{
//					idfr->Release();
//					idfr = NULL;
//				}
//			}

//			if(idf!=NULL)
//			{
//				idf->Release();
//				idf = NULL;
//			}

//		}
//	}
//}

//bool KinectCapture::CaptureColorFrame(IMultiSourceFrame *sf, int frame_count)
//{
//	if(sf==NULL)
//	{
//		return false;
//	}

//	bool ok = false;

//	IColorFrameReference *cfr = NULL;


//	if(SUCCEEDED(sf->get_ColorFrameReference(&cfr)))
//	{
//		IColorFrame* cf = NULL;
//		if(cfr && SUCCEEDED(cfr->AcquireFrame(&cf)))
//		{
//			HRESULT hr = cf->CopyConvertedFrameDataToArray(ColorImageSize,m_color_mat.data,ColorImageFormat_Bgra);
//			if(SUCCEEDED(hr))
//			{
//				ok = true;
//				//				if(frame_count==0)
//				//				{
//				////					m_db_item.start = QDateTime::currentDateTime().toString(m_tm_fmt);
//				////					m_db_item.type = "Color";
//				////					m_db_item.colorFileName = "Color_"+m_db_item.start+".m4v";
//				//////					m_db_item.dir = m_save_dir.path();

//				////					QString path = m_save_dir.absoluteFilePath(m_db_item.colorFileName);
//				//					qDebug()<<"Start recording "+m_db_item.colorFileName;

//				////					//m_encoder.createFile(path,640,360,1000000,40,8);


//				////					m_color_video_writer->open(path.toStdString().c_str(),CV_FOURCC('m','p','4','v'),8.0, cv::Size(640,360));

//				//																	 //cv::Size(704,576));
//				//				}

//				cv::Mat scaled_image, bgr;
//				cv::resize(m_color_mat,scaled_image,cv::Size(640,360),0,0,CV_INTER_NN);
//				cv::cvtColor(scaled_image,bgr,CV_BGRA2BGR);
//				m_color_video_writer->write(bgr);
//				//m_encoder.encodeImage(scaled_image.ptr<unsigned char>(), scaled_image.step1());
//			}

//			if(cfr!=NULL)
//			{
//				cfr->Release();
//				cfr = NULL;
//			}
//		}

//		if(cf!=NULL)
//		{
//			cf->Release();
//			cf = NULL;
//		}
//	}


//	return ok;
//}

bool KinectCapture::SaveAcquiredFrames(IColorFrame *cf, IDepthFrame *df)
{
	bool ok = false;

	if(cf!=NULL && df!=NULL)
	{

		//write depth
		if(ProcessColorFrame(cf)&&ProcessDepthFrame(df))
		{
			if(m_frame_count == 0)
			{
				//open file to write video
				m_db_item.start = QDateTime::currentDateTime().toString(m_tm_fmt);
				m_db_item.dir = m_save_dir.path();
				m_db_item.colorFileName = "Color_"+m_db_item.start+".m4v";
				qDebug()<<"Start Recording "<<m_db_item.colorFileName;
				QString path = m_save_dir.absoluteFilePath(m_db_item.colorFileName);
				m_color_video_writer->open(path.toStdString().c_str(),CV_FOURCC('m','p','4','v'),8.0, cv::Size(640,360));
				m_db_item.depthFileName = "Depth_"+m_db_item.start+".m4v";
				path = m_save_dir.absoluteFilePath(m_db_item.depthFileName);
				qDebug()<<"Start Recording "<<m_db_item.depthFileName;
				m_depth_video_writer->open(path.toStdString().c_str(),CV_FOURCC('m','p','4','v'),8.0,cv::Size(DepthImageWidth,DepthImageHeight));
			}

			m_depth_video_writer->write(m_depth_mat);
			m_color_video_writer->write(m_bgr_mat);
			++m_frame_count;
			if(m_frame_count%100==0)
			{
				qDebug()<<"Frame Count:"<<m_frame_count;
			}

			ok = true;
		}

		if(m_frame_count==m_max_frame_count)
		{
			m_db_item.end = QDateTime::currentDateTime().toString(m_tm_fmt);
			m_frame_count = 0;
			//m_encoder.close();

			if(m_depth_video_writer->isOpened())
			{
				m_depth_video_writer->release();
			}

			if(m_color_video_writer->isOpened())
			{
				m_color_video_writer->release();
			}
		}
	}

	return ok;
}

bool KinectCapture::ProcessDepthFrame(IDepthFrame *pDepthFrame)
{
	UINT16*	depthBuffer = NULL;

	UINT depthBufferSize = 0;

	USHORT maxReliableDistance = 0;
	USHORT minReliableDistance = 0;

	int depthWidth = 0; int depthHeight = 0;

	IFrameDescription* pDepthFrameDescription = NULL;

	HRESULT hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);

	if (SUCCEEDED(hr))
	{
		hr = pDepthFrameDescription->get_Width(&depthWidth);
	}
	if (SUCCEEDED(hr))
	{
		hr = pDepthFrameDescription->get_Height(&depthHeight);
	}
	if (SUCCEEDED(hr))
	{
		hr = pDepthFrame->get_DepthMaxReliableDistance(&maxReliableDistance);
	}
	if (SUCCEEDED(hr))
	{
		hr = pDepthFrame->get_DepthMinReliableDistance(&minReliableDistance);
	}
	if (SUCCEEDED(hr))
	{
		hr = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, &depthBuffer);
	}

	if (!SUCCEEDED(hr))
	{
		return false;
	}

	if(depthWidth == DepthImageWidth && depthHeight == DepthImageHeight)
	{
		ProcessDepthFrame(depthBuffer,minReliableDistance,maxReliableDistance);
		return true;
	}

	return false;
}

bool KinectCapture::ProcessColorFrame(IColorFrame *pColorFrame)
{
	IFrameDescription* pColorFrameDescription = NULL;
	HRESULT hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);

	int colorWidth = 0, colorHeight = 0;
	if (SUCCEEDED(hr))
	{
		hr = pColorFrameDescription->get_Width(&colorWidth);
	}
	if (SUCCEEDED(hr))
	{
		hr = pColorFrameDescription->get_Height(&colorHeight);
	}

	if(pColorFrameDescription!=NULL)
	{
		pColorFrameDescription->Release();
		pColorFrameDescription = NULL;
	}

	if (SUCCEEDED(hr))
	{

		if(colorWidth==ColorImageWidth && colorHeight == ColorImageHeight)
		{
			hr = pColorFrame->CopyConvertedFrameDataToArray(ColorImageSize,m_color_mat.data,ColorImageFormat_Bgra);
			if(SUCCEEDED(hr))
			{
				cv::resize(m_color_mat,m_scaled_mat,cv::Size(640,360),0,0,CV_INTER_NN);
				cv::cvtColor(m_scaled_mat,m_bgr_mat,CV_BGRA2BGR);
				return true;
			}
		}
	}

	return false;
}

void KinectCapture::SetSaveDirectory(const char *path)
{
	QString savePath = QString("%1").arg(path);
	qDebug()<<savePath;
	m_save_dir.setPath(QString(savePath));
	if(!m_save_dir.exists())
	{
		m_save_dir.mkdir(savePath);
	}
}

void KinectCapture::Release()
{
	if(_evt_frame_ready!=NULL)
	{
		m_pFrameReader->UnsubscribeMultiSourceFrameArrived(_evt_frame_ready);
		m_pFrameReader->Release();
	}

	if(m_pCoordinateMapper!=NULL)
	{
		m_pCoordinateMapper->Release();
		m_pCoordinateMapper = NULL;
	}
	if(m_pKinectSensor!=NULL)
	{
		m_pKinectSensor->Close();
		m_pKinectSensor->Release();
		m_pKinectSensor = NULL;
	}

	if(m_color_video_writer->isOpened())
	{
		m_color_video_writer->release();
	}

	if(m_depth_video_writer->isOpened())
	{
		m_depth_video_writer->release();
	}

	delete[] m_pColorSpacePoints;

}

//void KinectCapture::WriteVideo()
//{
////    if(!_cvw->isOpened())
////    {
////       bool retval =  _cvw->open(path,9,8.0,cv::Size(KinectCapture::Width,KinectCapture::Height));
////       qDebug()<<retval;
////    }

//    _cvw->write(_color_image);

//}

//bool KinectCapture::StartWriteVideo(const char *path)
//{
//    return _cvw->open(path,9,8.0,cv::Size(ColorImageWidth,ColorImageHeight));
//}

//void KinectCapture::EndWriteVideo()
//{
//    //_cvw->release();
//}

//void

//bool KinectCapture::CopyColorFrameData(IColorFrame* frame)
//{
//   HRESULT hr = frame->CopyConvertedFrameDataToArray(_color_image_size,_color_image.data,ColorImageFormat_Bgra);
//   return SUCCEEDED(hr);
//}

