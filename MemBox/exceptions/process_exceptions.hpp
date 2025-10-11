#pragma once
#include <iostream>
#include "exception.hpp"

namespace MemBox
{
	class ProcessAttachException : public MemBoxException
	{
		std::string message;
	public:
		explicit ProcessAttachException(const std::string& pid, const std::string& errorCode)
			: message("Failed to attach process with pid: " + pid + ". Error code : " + errorCode) {
		}

		const char* what() const noexcept override
		{
			return message.c_str();
		}
	};

	class ProcessNotFoundException : public MemBoxException
	{
		std::string message;
	public:
		explicit ProcessNotFoundException(const std::string& processName)
			: message("Could not find process " + processName) {
		}

		const char* what() const noexcept override
		{
			return message.c_str();
		}
	};
}
