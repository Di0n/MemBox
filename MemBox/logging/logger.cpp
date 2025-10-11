#include "pch.h"
#include "logger.hpp"
#include <iostream>
#include <mutex>

namespace MemBox
{
    static LogCallback currentLogger = nullptr;
    static std::mutex logMutex;

    void SetLogger(LogCallback callback)
    {
        std::lock_guard lock(logMutex);
        currentLogger = callback;
    }

    void Log(LogLevel level, const std::string& message)
    {
        std::lock_guard lock(logMutex);

        if (currentLogger)
        {
            currentLogger(level, message);
            return;
        }

        // Default fallback (stdout)
        std::string levelStr = "";
        switch (level)
        {
            case LogLevel::Debug:   levelStr = "[DEBUG]"; break;
            case LogLevel::Info:    levelStr = "[INFO]"; break;
            case LogLevel::Warning: levelStr = "[WARN]"; break;
            case LogLevel::Error:   levelStr = "[ERROR]"; break;
        }

        std::cerr << levelStr << " " << message << std::endl;
    }
}