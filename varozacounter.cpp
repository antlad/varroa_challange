#include "varozacounter.hpp"
#include "common.hpp"
#include "log.hpp"

#include <boost/throw_exception.hpp>

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

VarozaCounter::VarozaCounter(
	Features ftype
)
	: m_matcher(cv::DescriptorMatcher::create("BruteForce"))
{
	switch (ftype)
	{
	case Features::SIFT:
	{
		m_features = cv::xfeatures2d::SIFT::create(
						 0,
						 3,
						 0.01,
						 10,
						 1.6);
		break;
	}

	case Features::ORB:
	{
		break;
	}
	}
}

void VarozaCounter::addTrainImg(const cv::Mat& img)
{
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	m_features->detectAndCompute(img, cv::Mat(), keypoints, descriptors);
	m_trainDescriptors.push_back(descriptors);
}

void VarozaCounter::trainMatch()
{
	if (m_trainDescriptors.empty())
	{
		BOOST_THROW_EXCEPTION(std::runtime_error("train descriptors empty, check logic"));
	}

	m_matcher->add(m_trainDescriptors);
	m_matcher->train();
}

void VarozaCounter::loadMatchStorage(const std::string& matchStoragePath)
{
	m_matcher->read(matchStoragePath);
}

void VarozaCounter::saveMatchStorage(const std::string& matchStoragePath)
{
	m_matcher->write(matchStoragePath);
}


VarozaCounter::~VarozaCounter()
{
}

int VarozaCounter::processFrame(const cv::Mat& img, const cv::Mat& original_large)
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
		//
		std::vector<cv::KeyPoint> keypoints_o;
		cv::Mat descriptors_o;
		cv::Mat original;
		double scale = 100.0 / original_large.size().width;
		cv::resize(original_large, original, cv::Size(), scale, scale);
		//original_large.
		m_features->detectAndCompute(original, cv::Mat(), keypoints_o, descriptors_o);
		//
		std::vector<std::vector<cv::DMatch>> matches;
		m_matcher->knnMatch(descriptors, descriptors_o, matches, 2);
		//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
		//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
		//-- small)
		//-- PS.- radiusMatch can also be used here.
		std::vector<cv::DMatch> good_matches;

		for (const auto& o : matches)
		{
			for (const auto& e : o)
			{
				good_matches.push_back(e);
			}
		}

		LOG_TRACE << "matches size " << good_matches.size();
		//		cv::InputArray img1, std::vector<cv::KeyPoint> keypoints1,
		//					cv::InputArray img2, std::vector<cv::KeyPoint> keypoints2,
		//					std::vector<cv::DMatch> matches, const impl::CallMetaData &data,
		//					const std::string &description, const std::string &view,
		//					bool useTrainDescriptor = true)
		//-- Draw only "good" matches
		cv::Mat img_matches;
		cvv::debugDMatch(img, keypoints, original, keypoints_o, good_matches, CVVISUAL_LOCATION);
		//		drawMatches(img_1, keypoints_1, img_2, keypoints_2,
		//					good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		//					vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		//		cv::cvtColor(img, gray, CV_BGR2GRAY);
		//		cv::Mat t;
		//		cv::adaptiveThreshold(gray, t, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 31, -2);
		//		cvv::debugFilter(gray, t, CVVISUAL_LOCATION, "adaptiveThreshold");
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
