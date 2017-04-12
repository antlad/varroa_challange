#include "varozacounter.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#ifdef CVVISUAL_DEBUGMODE
#include <opencv2/cvv/debug_mode.hpp>
#include <opencv2/cvv/show_image.hpp>
#include <opencv2/cvv/filter.hpp>
#include <opencv2/cvv/dmatch.hpp>
#include <opencv2/cvv/final_show.hpp>
#endif

namespace varoza {

VarozaCounter::VarozaCounter(Features ftype)
{
	switch (ftype)
	{
	case Features::SIFT:
	{
		m_features = cv::xfeatures2d::SIFT::create();
		break;
	}

	case Features::ORB:
	{
		break;
	}
	}
}

VarozaCounter::~VarozaCounter()
{
}

int VarozaCounter::processFrame(const cv::Mat& img)
{
#ifdef CVVISUAL_DEBUGMODE

	try
	{
		cvv::showImage(img, CVVISUAL_LOCATION,  "Base image");
#endif
		std::vector<cv::KeyPoint> keypoints;
		cv::Mat descriptors;
		m_features->detectAndCompute(img, cv::Mat(), keypoints, descriptors);
#ifdef CVVISUAL_DEBUGMODE
		cv::Mat keypoints_draw;
		cv::drawKeypoints(img, keypoints, keypoints_draw);
		cvv::showImage(keypoints_draw, CVVISUAL_LOCATION,  "Found keypoints");
#endif
#ifdef CVVISUAL_DEBUGMODE
		cvv::finalShow();
	}
	catch (...)
	{
		cvv::finalShow();
		throw;
	}

#endif
	return 0;
}


}
