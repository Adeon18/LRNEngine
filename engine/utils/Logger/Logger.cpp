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
		std::filesystem::resize_file(logFilePath, 0);
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

	void Logger::logInfo(const std::string& s, int c)
	{
		if (m_defaultLoggerName != "C") {
			spdlog::get("C")->info(s + "{}", c);
		}
		spdlog::get(m_defaultLoggerName)->info(s + "{}", c);
	}

	void Logger::logWarn(const std::string& s)
	{
		spdlog::get(m_defaultLoggerName)->warn(s);
	}
	void Logger::logErr(const std::string& s)
	{
		spdlog::get(m_defaultLoggerName)->error(s);
	}
	void Logger::logCrit(const std::string& s)
	{
		spdlog::get(m_defaultLoggerName)->critical(s);
	}
	void Logger::logDebug(const std::string& s)
	{
		spdlog::get("C")->debug(s);
	}
	void Logger::logDebug(const std::string& s, int c)
	{
		spdlog::get("C")->debug(s + ": {}", c);
	}
} // engn