#include "varozacounter.hpp"
#include "log.hpp"

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

namespace po = boost::program_options;
namespace bf = boost::filesystem;

int main(int argc, char* argv[])
{
	try
	{
		std::string inputFileName;
		po::options_description desc("Allowed options");
		desc.add_options()
		("f", po::value<std::string>(&inputFileName), "input file")
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
		varoza::VarozaCounter counter;
		auto img = cv::imread(inputFileName);
		auto now  = boost::posix_time::second_clock::local_time();
		LOG_TRACE << now;
		counter.processFrame(img);
		auto after  = boost::posix_time::second_clock::local_time();
		LOG_TRACE << after;
		std::cout << "0" << std::endl;
	}
	catch (const std::exception&)
	{
		std::cerr << boost::current_exception_diagnostic_information(true);
		return 1;
	}

	return 0;
}
