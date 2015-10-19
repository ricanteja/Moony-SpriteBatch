#ifndef LOG_H
#define LOG_H

#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


// Get the current time in a formatted string
std::string logGetTimestamp()
{
	time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm time_properties;
	char buffer[32];
	std::string time_stamp_str;

	localtime_s(&time_properties, &current_time);
	asctime_s(buffer, 32, &time_properties);
	time_stamp_str = buffer;
	time_stamp_str.erase(time_stamp_str.rfind('\n'));

	return time_stamp_str;
}

// Get the log stream
std::stringstream& logGetStream()
{
	static std::stringstream stream;
	return stream;
}

// Get the log stream for error messages
std::stringstream& logError()
{
	logGetStream() << "ERROR--" << logGetTimestamp() << "--";
	return logGetStream();
}

// Get the log stream for debug messages
std::stringstream& logDebug()
{
	logGetStream() << "DEBUG--" << logGetTimestamp() << "--";
	return logGetStream();
}

// Write the log stream to a file
void logSaveToFile(const std::string file, std::ios::openmode mode)
{
	std::ofstream file_stream(file, mode);

	if(!file_stream.bad())
		file_stream << logGetStream().str();
	else
		std::cerr << "Failed to create log file\n";
	
	file_stream.close();
}

#endif