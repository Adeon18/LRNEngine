#include "Logger.hpp"


namespace engn {
	void Logger::initConsoleLogger()
	{
		auto console = spdlog::stdout_color_mt("C");
		console->info("Initialized console logger");
		// We always enable debug
		enableDebug();
	}
	void Logger::createFileLogger(const std::string& name, const std::string& path)
	{
		std::string logFilePath = util::getExeDir() + path;
		if (std::filesystem::exists(logFilePath)) {
			std::filesystem::resize_file(logFilePath, 0);
		}
		auto logger = spdlog::basic_logger_mt(name, logFilePath);
		logger->info("Initialized file logger");
	}
	void Logger::logInfo(const std::string& s)
	{
		if (m_defaultLoggerName != "C") {
			spdlog::get("C")->info(s);
		}
		spdlog::get(m_defaultLoggerName)->info(s);
	}

	void Logger::logInfo(const std::wstring& s)
	{
		logInfo(std::string{ s.begin(), s.end() });
	}

	void Logger::logInfo(const std::string& s, int c)
	{
		if (m_defaultLoggerName != "C") {
			spdlog::get("C")->info(s + "{}", c);
		}
		spdlog::get(m_defaultLoggerName)->info(s + "{}", c);
	}

	void Logger::logInfo(const std::wstring& s, int c)
	{
		logInfo(std::string{ s.begin(), s.end() }, c);
	}

	void Logger::logWarn(const std::string& s)
	{
		spdlog::get(m_defaultLoggerName)->warn(s);
	}
	void Logger::logWarn(const std::wstring& s)
	{
		logWarn(std::string{ s.begin(), s.end() });
	}
	void Logger::logErr(const std::string& s)
	{
		spdlog::get(m_defaultLoggerName)->error(s);
	}
	void Logger::logErr(const std::wstring& s)
	{
		logErr(std::string{ s.begin(), s.end() });
	}
	void Logger::logCrit(const std::string& s)
	{
		spdlog::get(m_defaultLoggerName)->critical(s);
	}
	void Logger::logCrit(const std::wstring& s)
	{
		logCrit(std::string{ s.begin(), s.end() });
	}
	void Logger::logDebug(const std::string& s)
	{
		spdlog::get("C")->debug(s);
	}
	void Logger::logDebug(const std::wstring& s)
	{
		logDebug(std::string{ s.begin(), s.end() });
	}
	void Logger::logDebug(const std::string& s, int c)
	{
		spdlog::get("C")->debug(s + ": {}", c);
	}
	void Logger::logDebug(const std::wstring& s, int c)
	{
		logDebug(std::string{ s.begin(), s.end() }, c);
	}
} // engn