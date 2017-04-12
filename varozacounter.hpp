#ifndef VAROZACOUNTER_HPP
#define VAROZACOUNTER_HPP

#include <opencv2/core.hpp>

#include <opencv2/features2d.hpp>

namespace varoza {

class VarozaCounter {
public:

	enum class Features
	{
		SIFT,
		ORB
	};

	VarozaCounter(Features ftype = Features::SIFT);

	virtual ~VarozaCounter();

	/*! Will count varoza at image and return
	 * \param img Frame to process
	 * \return Count of varoza
	 */
	virtual int processFrame(const cv::Mat& img);

private:
	cv::Ptr<cv::Feature2D> m_features;
};

}

#endif // VAROZACOUNTER_HPP
