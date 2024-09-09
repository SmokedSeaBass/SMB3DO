#include "logger.hpp"

#include <iostream>
#include <sstream>
#include "constants.hpp"

void Logger::PrintError(const std::string& message)
{
	std::cout
	<< Color::GetAnsiCode(Color::ColorName::red)
	<< "[ERROR] "
	<< Color::GetAnsiCode(Color::ColorName::white)
	<< message
	<< std::endl;
}

void Logger::PrintError(
	const std::string& message,
	const std::string& file,
	const int line
)
{
	std::cout
	<< Color::GetAnsiCode(Color::ColorName::red)
	<< "[ERROR] "
	<< Color::GetAnsiCode(Color::ColorName::white)
	<< message
	<< "\n * " << file << " | line " << std::to_string(line)
	<< std::endl;
}

void Logger::PrintError(const std::exception exception)
{
	std::cout
	<< Color::GetAnsiCode(Color::ColorName::red)
	<< "[ERROR] "
	<< Color::GetAnsiCode(Color::ColorName::white)
	<< exception.what()
	<< std::endl;
}

void Logger::PrintError(
	const std::exception exception,
	const std::string& file,
	const int line
)
{
	std::cout
	<< Color::GetAnsiCode(Color::ColorName::red)
	<< "[ERROR] "
	<< Color::GetAnsiCode(Color::ColorName::white)
	<< exception.what()
	<< "\n * " << file << " | line " << std::to_string(line)
	<< std::endl;
}

void Logger::PrintWarning(const std::string& message)
{
	std::cout
	<< Color::GetAnsiCode(Color::ColorName::yellow)
	<< "[WARN] "
	<< Color::GetAnsiCode(Color::ColorName::white)
	<< message
	<< std::endl;
}

void Logger::PrintInfo(const std::string& message)
{
	std::cout
	<< Color::GetAnsiCode(Color::ColorName::cyan)
	<< "[INFO] "
	<< Color::GetAnsiCode(Color::ColorName::white)
	<< message
	<< std::endl;
}

void Logger::PrintDebug(const std::string& message)
{
	if (META_DEBUG) {
		std::cout
		<< Color::GetAnsiCode(Color::ColorName::magenta)
		<< "[DEBUG] "
		<< Color::GetAnsiCode(Color::ColorName::white)
		<< message
		<< std::endl;
	}
}

std::string Logger::PointerToString(void* ptr)
{
	const void* addr = static_cast<const void*>(ptr);
	std::stringstream stream;
	stream << addr;
	return stream.str();
}

// void Logger::LogFormattedMessage(
// 	const std::string& label,
// 	const Color::ColorName label_color,
// 	const std::string& message
// )
// {
// 	std::cout
// 	<< Color::GetAnsiCode(label_color)
// 	<< "[" << label << "] "
// 	<< Color::GetAnsiCode(Color::ColorName::white)
// 	<< message
// 	<< std::endl;
// }
