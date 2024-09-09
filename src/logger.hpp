#pragma once

#include <string>
#include <stdexcept>
#include "color.hpp"


class Logger {
public:
	static void PrintError(const std::string& message);
	static void PrintError(
		const std::string& message,
		const std::string& file,
		const int line
    );
	static void PrintError(const std::exception exception);
	static void PrintError(
		const std::exception exception,
		const std::string& file,
		const int line
    );
	static void PrintWarning(const std::string& message);
	static void PrintInfo(const std::string& message);
	static void PrintDebug(const std::string& message);
	static std::string PointerToString(void* ptr);
private:
	// static void LogFormattedMessage(
	// 	const std::string& label,
    //     const Color::ColorName label_color,
	// 	const std::string& message
    // );
};