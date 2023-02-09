#pragma once

#include <filesystem>
#include <system_error>

#include <spdlog/include/spdlog/spdlog.h>
#include <spdlog/include/spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/include/spdlog/sinks/basic_file_sink.h>

#include "include/utility/utility.hpp"

namespace engn {
	class Logger {
	public:
		static Logger& instance() {
			static Logger l;
			return l;
		}

		//! Initialize the console logger -> MUST be called before using logger for the first time
		static void initConsoleLogger();
		//! Create a new file logger that will write in a clean file(CLEARS THE FILE AT PATH!)
		static void createFileLogger(const std::string& name, const std::string& path);
		//! enable debug output
		static void enableDebug() { spdlog::set_level(spdlog::level::debug); }

		//! Set the default logger name - which logger will write errors by default
		void setDefaultLoggerName(const std::string& name) { m_defaultLoggerName = name; }

		//! Log data with INFO tag - goes to DEFAULT LOGGER and COUT both
		void logInfo(const std::string& s);
		void logInfo(const std::wstring& s);
		void logInfo(const std::string& s, int c);
		void logInfo(const std::wstring& s, int c);
		//! Log data with WARN tag - goes to DEFAULT LOGGER
		void logWarn(const std::string& s);
		void logWarn(const std::wstring& s);
		//! Log data with ERR tag - goes to DEFAULT LOGGER
		void logErr(const std::string& s);
		void logErr(const std::wstring& s);
		//! Log data with CRIT tag - goes to DEFAULT LOGGER
		void logCrit(const std::string& s);
		void logCrit(const std::wstring& s);

		//! Log data with DEBUG tag - goes to cout by default
		void logDebug(const std::string& s);
		void logDebug(const std::wstring& s);
		void logDebug(const std::string& s, int c);
		void logDebug(const std::wstring& s, int c);

		Logger(const Logger& l) = delete;
		Logger& operator=(const Logger& l) = delete;
	private:
		Logger() {}

		std::string m_defaultLoggerName = "C";
	};
} // engn