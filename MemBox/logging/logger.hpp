#pragma once
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

	/// <summary>
	/// Sets logger callback function
	/// </summary>
	/// <param name="callback"></param>
	void SetLogger(LogCallback callback);

	/// <summary>
	/// Log a message with a specified log level
	/// </summary>
	/// <param name="level"></param>
	/// <param name="message"></param>
	void Log(LogLevel level, const std::string& message);
}
