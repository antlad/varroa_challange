#include "varozacounter.hpp"
#include "common.hpp"
#include "log.hpp"

#include <boost/throw_exception.hpp>
#include <boost/format.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#ifdef CVVISUAL_DEBUGMODE
#include <opencv2/cvv/debug_mode.hpp>
#include <opencv2/cvv/show_image.hpp>
#include <opencv2/cvv/filter.hpp>
#include <opencv2/cvv/dmatch.hpp>
#include <opencv2/cvv/final_show.hpp>
#endif

namespace varoza {

VarozaCounter::VarozaCounter(
	const std::string& cascadeFilterModel,
	const CascadeParams& params
)
	: m_filter(new cv::CascadeClassifier(cascadeFilterModel))
	, m_params(params)
{
	LOG_TRACE << "VarozaCounter()";
}


VarozaCounter::~VarozaCounter()
{
}


int VarozaCounter::processFrame(const cv::Mat& original)
{
	int count = 0;
#ifdef CVVISUAL_DEBUGMODE

	try
	{
		cvv::showImage(original, CVVISUAL_LOCATION,  "Base image");
#endif
		cv::Mat gray;

		if (original.channels() == 3)
		{
			cvtColor(original, gray, CV_BGR2GRAY);
		}
		else if (original.channels() == 4)
		{
			cvtColor(original, gray, CV_BGRA2GRAY);
		}
		else
		{
			gray = original;
		}

		cv::Mat img;
		cv::equalizeHist(gray, img);
#ifdef CVVISUAL_DEBUGMODE
		cvv::showImage(img, CVVISUAL_LOCATION,  "equalizeHist");
#endif
		std::vector<cv::Rect> rects;
		std::vector<int> rejectLevels;
		std::vector<double> levelWeights;
		m_filter->detectMultiScale(img, rects,
								   rejectLevels,
								   levelWeights,
								   m_params.scaleFactor,
								   m_params.minNeighbors,
								   0,
								   m_params.minSize,
								   m_params.maxSize,
								   true);
		std::vector<cv::Rect> goodRects;
		int i = 0;

		for (const auto& r : rects)
		{
			if (levelWeights[i] > 0)
			{
				goodRects.push_back(r);
			}

			++i;
		}

		count = goodRects.size();
#ifdef CVVISUAL_DEBUGMODE
		cv::Mat draw;
		original.copyTo(draw);

		for (const auto& r : goodRects)
		{
			cv::rectangle(draw, r, cv::Scalar(0, 255, 0), 3);
		}

		cvv::showImage(draw, CVVISUAL_LOCATION,  "Rectangles");
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
	return count;
}

CascadeParams::CascadeParams()
{
}

CascadeParams::CascadeParams(double scaleFactor, int minNeighbors, cv::Size minSize, cv::Size maxSize)
	: scaleFactor(scaleFactor)
	, minNeighbors(minNeighbors)
	, minSize(minSize)
	, maxSize(maxSize)
{
}


}
