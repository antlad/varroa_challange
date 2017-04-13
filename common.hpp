#pragma once

#include <chrono>
#include <vector>
#include <string>

namespace utils {



class TimerElapsed {
public:
	TimerElapsed();
	std::size_t elapsedMiliseconds() const;
private:
	std::chrono::steady_clock::time_point m_start;
};

std::vector<std::string> listOfFiles(const std::string& folder);

}
