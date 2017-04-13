#include "varozacounter.hpp"
#include "log.hpp"
#include "common.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/date_time.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <string>

//http://eric-yuan.me/sift/
//https://github.com/opencv/opencv/blob/2.4/samples/cpp/matching_to_many_images.cpp

//http://docs.opencv.org/trunk/dc/d88/tutorial_traincascade.html
//http://docs.opencv.org/2.4.13.2/doc/user_guide/ug_traincascade.html

namespace po = boost::program_options;
namespace bf = boost::filesystem;


int main(int argc, char* argv[])
{
	try
	{
		std::string inputFileName;
		std::string matchFileStorage;
		std::string originalFolder;
		std::string mode("search");
		std::string extractFolder;
		po::options_description desc("Allowed options");
		std::string logLevel("error");
		desc.add_options()
		("f", po::value<std::string>(&inputFileName)->required(), "input file")
		("match_storage", po::value<std::string>(&matchFileStorage), "")
		("original_folder", po::value<std::string>(&originalFolder), "if non empty - rebuild train from this folder")
		("mode", po::value<std::string>(&mode), "mode of useing. default 'search'. Possible extract")
		("extract_folder", po::value<std::string>(&extractFolder), "output folder for extract mode")
		("log_level", po::value<std::string>(&logLevel), "log level (debug, info, warn, error, fatal)")
		("help", "produce help message");
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << std::endl;
			return 0;
		}

		lg::initializeJustConsole();
		lg::setLogLevelFromString(logLevel);
		auto pathExistCheck = [](const std::string & path)
		{
			if (!bf::exists(path))
			{
				const auto& msg = (boost::format("File path %s doesn't exsists") % path).str();
				BOOST_THROW_EXCEPTION(std::runtime_error(msg));
			}
		};
		varoza::VarozaCounter counter;

		if (!originalFolder.empty())
		{
			pathExistCheck(originalFolder);

			for (const auto& f : listOfFiles(originalFolder))
			{
				LOG_TRACE << "reading file: " << f;
				auto img = cv::imread(f);
				LOG_TRACE << "add image: " << f;
				counter.addTrainImg(img);
			}

			LOG_TRACE << "trainging start";
			counter.trainMatch();
			//			LOG_TRACE << "trainging done";
			//			counter.saveMatchStorage(matchFileStorage);
		}

		utils::TimerElapsed t;
		auto img = cv::imread(inputFileName);
		auto o = cv::imread("/home/vlad/projects/varoza_challange/original/varroa_24.jpg");
		LOG_TRACE << "imput read done " <<  t.elapsedMiliseconds() << " ms";
		counter.processFrame(img, o);
		LOG_TRACE << "process done " <<  t.elapsedMiliseconds() << " ms";
		std::cout << "0" << std::endl;
	}
	catch (const std::exception&)
	{
		std::cerr << boost::current_exception_diagnostic_information(true);
		return 1;
	}

	return 0;
}
