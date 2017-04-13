#pragma once

#include <opencv2/core.hpp>

#include <opencv2/features2d.hpp>

#include <vector>

namespace varoza {

class VarozaCounter {
public:

	enum class Features
	{
		SIFT,
		ORB
	};

	VarozaCounter(
		Features ftype = Features::SIFT
	);

	virtual void addTrainImg(const cv::Mat& img);

	virtual void trainMatch();

	virtual void loadMatchStorage(const std::string& matchStoragePath);

	virtual void saveMatchStorage(const std::string& matchStoragePath);

	virtual ~VarozaCounter();

	/*! Will count varoza at image and return
	 * \param img Frame to process
	 * \return Count of varoza
	 */
	virtual int processFrame(const cv::Mat& img, const cv::Mat& original);

private:
	cv::Ptr<cv::Feature2D> m_features;
	cv::Ptr<cv::DescriptorMatcher> m_matcher;

	std::vector<cv::Mat> m_trainDescriptors;
};

}
