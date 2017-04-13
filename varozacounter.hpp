#pragma once

#include <opencv2/core.hpp>

#include <vector>
#include <memory>

namespace cv {
class CascadeClassifier;

}

namespace varoza {

struct CascadeParams
{
	CascadeParams();
	CascadeParams(double scaleFactor,
				  int minNeighbors,
				  cv::Size minSize ,
				  cv::Size maxSize);
	double scaleFactor = 1.1;
	int minNeighbors = 5;
	cv::Size minSize = cv::Size(20, 20);
	cv::Size maxSize = cv::Size(200, 200);
};

class VarozaCounter {
public:

	/*! Path to cascade filter model
	 * \param cascadeFilterModel
	 */
	VarozaCounter(
		const std::string& cascadeFilterModel,
		const CascadeParams& params = CascadeParams()
	);

	virtual ~VarozaCounter();

	/*! Will count varoza at image and return
	 * \param img Frame to process
	 * \return Count of varoza
	 */
	virtual int processFrame(const cv::Mat& img);

private:
	std::unique_ptr<cv::CascadeClassifier> m_filter;
	CascadeParams m_params;
};

}
