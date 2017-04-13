#pragma once

#include <opencv2/core.hpp>

#include <vector>

namespace utils {

void processBackgroudAnnotationFile(const std::string& filePath, const std::string& outputFolder);

std::vector<cv::Mat> cropImageRefs(const cv::Mat& source, const std::vector<cv::Rect>& crops);


}
