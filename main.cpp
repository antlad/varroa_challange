#include "varozacounter.hpp"
#include "log.hpp"
#include "common.hpp"
#include "extract_background.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/date_time.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "json.hpp"

#include <mutex>
#include <thread>
#include <iostream>
#include <string>

namespace po = boost::program_options;
namespace bf = boost::filesystem;


int main(int argc, char* argv[])
{
	try
	{
		std::string inputFileName;
		std::string inputFolder;
		std::string modelFile;
		std::string mode("search");
		std::string extractFolder;
		std::string jsonOut;
		double scaleFactor = 1.1;
		int minNeighbors = 3;
		int minSize = 20;
		int maxSize = 200;
		size_t threadsCount = std::thread::hardware_concurrency();
		po::options_description desc("Allowed options");
		std::string logLevel("error");
		desc.add_options()
		("f", po::value<std::string>(&inputFileName), "input file")
		("input_folder", po::value<std::string>(&inputFolder), "input folder (used in search_in_folder mode)")
		("json_out", po::value<std::string>(&jsonOut), "search in folder results file (used in search_in_folder mode)")
		("model", po::value<std::string>(&modelFile), "cascade filter model file")
		("mode", po::value<std::string>(&mode), "mode of using, default 'search'. Possible extract | search_in_folder")
		("extract_folder", po::value<std::string>(&extractFolder), "output folder for extract mode")
		("cascade_scale", po::value<double>(&scaleFactor), "cascade scale")
		("cascade_neighbors", po::value<int>(&minNeighbors), "cascade neighbors")
		("cascade_min_size", po::value<int>(&minSize), "cascade min size")
		("cascade_max_size", po::value<int>(&maxSize), "cascade max size")
		("threads_count", po::value<size_t>(&threadsCount), "threads count (used in search_in_folder mode)")
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

		if (mode == "extract")
		{
			utils::processBackgroudAnnotationFile(inputFileName, extractFolder);
			return 0;
		}
		else if (mode == "search")
		{
			if (modelFile.empty())
			{
				BOOST_THROW_EXCEPTION(std::runtime_error("Please set up cascade filter file"));
			}

			utils::pathExistCheck(modelFile);
			varoza::VarozaCounter counter(modelFile,
										  varoza::CascadeParams(scaleFactor, minNeighbors,
												  cv::Size(minSize, minSize),
												  cv::Size(maxSize, maxSize)));
			utils::TimerElapsed t;
			auto img = cv::imread(inputFileName);
			LOG_TRACE << "imput read done " <<  t.elapsedMiliseconds() << " ms";
			int count = counter.processFrame(img);
			std::cout << count;
			LOG_TRACE << "process done " <<  t.elapsedMiliseconds() << " ms";
		}
		else if (mode == "search_in_folder")
		{
			if (modelFile.empty())
			{
				BOOST_THROW_EXCEPTION(std::runtime_error("Please set up cascade filter file"));
			}

			if (jsonOut.empty())
			{
				BOOST_THROW_EXCEPTION(std::runtime_error("Please set up json output file"));
			}

			if (inputFolder.empty())
			{
				BOOST_THROW_EXCEPTION(std::runtime_error("Please set up input folder path"));
			}

			utils::pathExistCheck(modelFile);
			utils::pathExistCheck(inputFolder);
			auto files = utils::listOfFiles(inputFolder);
			size_t total = files.size();
			struct Result
			{
				std::string filepath;
				int count;
			};
			std::vector<Result> results;
			std::mutex lock;
			#pragma omp parallel for num_threads(threadsCount)

			for (size_t i = 0; i < total; ++i)
			{
				thread_local varoza::VarozaCounter counter(modelFile,
						varoza::CascadeParams(scaleFactor, minNeighbors,
											  cv::Size(minSize, minSize),
											  cv::Size(maxSize, maxSize)));

				try
				{
					Result r;
					r.filepath = files[i];
					LOG_TRACE << "processing " << r.filepath;
					cv::Mat img = cv::imread(r.filepath);
					r.count = counter.processFrame(img);
					LOG_TRACE << "processing " << r.filepath << " done";
					std::lock_guard<std::mutex> g(lock);
					results.push_back(r);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR << boost::current_exception_diagnostic_information(true);
				}
			}

			nlohmann::json result_json;

			for (const auto& e : results)
			{
				nlohmann::json je;
				je["filepath"] = e.filepath;
				je["count"] = e.count;
				result_json.push_back(je);
			}

			std::ofstream of;
			of.open(jsonOut, std::ofstream::out);
			of << result_json;
		}
		else
		{
			BOOST_THROW_EXCEPTION(std::runtime_error((boost::format("Mode %s unsupported") % mode).str()));
		}
	}
	catch (const std::exception&)
	{
		std::cerr << boost::current_exception_diagnostic_information(true);
		return 1;
	}

	return 0;
}
