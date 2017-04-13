#include "extract_background.hpp"
#include "common.hpp"

namespace utils {

std::vector<cv::Mat> cropImagesRefs(const cv::Mat& source, const std::vector<cv::Rect>& crops)
{
	std::vector<cv::Mat> r;
	r.reserve(crops.size());

	for (const auto& c : crops)
	{
		source.c
	}

	return r;
}

void processBackgroudAnnotationFile(const std::string& filePath, const std::string& outputFolder)
{
}

}
