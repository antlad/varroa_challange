#include "extract_background.hpp"
#include "log.hpp"

#include <opencv2/imgcodecs.hpp>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "common.hpp"
#include <fstream>

namespace utils {

namespace bf = boost::filesystem;

std::vector<cv::Mat> cropImagesRefs(const cv::Mat& source, const std::vector<cv::Rect>& crops)
{
	std::vector<cv::Mat> r;
	r.reserve(crops.size());

	for (const auto& c : crops)
	{
		r.emplace_back(source(c));
	}

	return r;
}

void processBackgroudAnnotationFile(const std::string& filePath, const std::string& outputFolder)
{
	pathExistCheck(filePath);
	pathExistCheck(outputFolder);
	std::ifstream f;
	std::ofstream newAnnotation;
	f.open(filePath);
	newAnnotation.open((bf::path(outputFolder) / "bg.txt").string());
	int backgroundCounter = 0;

	while (!f.eof())
	{
		if (f.eof())
		{
			break;
		}

		std::string imagePath;
		f >> imagePath;
		pathExistCheck(imagePath);
		LOG_INFO << "reading " << imagePath;
		auto img = cv::imread(imagePath);
		int cropsCount = 0;
		f >> cropsCount;
		std::vector<cv::Rect> rects;
		rects.reserve(cropsCount);

		for (int i = 0; i < cropsCount; ++i)
		{
			int x = 0, y = 0,  w = 0, h = 0;
			f >> x >> y >> w >> h;
			rects.emplace_back(x, y, w, h);
		}

		auto refs = cropImagesRefs(img, rects);

		for (const auto& r : refs)
		{
			auto filename = (boost::format("background%d.png") % (backgroundCounter++)).str();
			auto outPath = (bf::path(outputFolder) / filename).string();
			newAnnotation << outPath << "\n";
			LOG_INFO << "saving crop " << outPath;
			cv::imwrite(outPath, r);
		}

		LOG_INFO << "crops saved for " << imagePath;
	}
}

}
