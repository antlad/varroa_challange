#include "common.hpp"

#include <boost/filesystem.hpp>

namespace utils {

namespace bf = boost::filesystem;

TimerElapsed::TimerElapsed()
	: m_start(std::chrono::steady_clock::now())
{
}

std::size_t TimerElapsed::elapsedMiliseconds() const
{
	std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
	return (size_t) std::chrono::duration_cast<std::chrono::milliseconds>(current - m_start).count();
}

std::vector<std::__cxx11::string> listOfFiles(const std::__cxx11::string& folder)
{
	std::vector<std::string> files;
	bf::path p(folder);
	bf::directory_iterator end;

	for (bf::directory_iterator it(p) ; it	 != end ; ++it)
	{
		if (bf::is_regular_file(it->status()))
		{
			files.emplace_back(it->path().string());
		}
	}

	return files;
}

}
