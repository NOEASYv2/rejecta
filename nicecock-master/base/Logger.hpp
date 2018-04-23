#pragma once

#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <memory>
#include <random>

namespace Logging
{
	class FileLogger
	{

	public:

		FileLogger(std::string fileNameIn, std::string fileNameOut);
		~FileLogger() { fileOut.close(); }

		void readFile(std::string &out) = delete;
		void write(const char *fmt, ...);

	private:

		std::string fileNameIn;
		std::string fileNameOut;

		std::ifstream fileIn;
		std::ofstream fileOut;
	};
}