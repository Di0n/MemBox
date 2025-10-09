#pragma once
#include "pch.h"
#include <functional>

namespace MemBox
{
	enum class LogLevel
	{
		Debug,
		Info,
		Warning,
		Error
	};

	using LogCallback = std::function<void(LogLevel, const std::string&)>;

	void SetLogger(LogCallback callback);
	void Log(LogLevel level, const std::string& message);
}
