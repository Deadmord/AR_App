#pragma once
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

// Define ANSI escape codes for text colors
const std::string ANSI_RESET = "\033[0m";
const std::string ANSI_RED = "\033[31m";
const std::string ANSI_GREEN = "\033[32m";
const std::string ANSI_YELLOW = "\033[33m";
const std::string ANSI_BLUE = "\033[34m";

class Console {
public:
    // Define a special nested class for handling output with color
    class LogStream {
    public:
        LogStream(std::ostream& stream, const std::string& color) : stream_(stream), color_(color) {}

        template <typename T>
        LogStream& operator<<(const T& value) {
            stream_ << color_ << value << ANSI_RESET; // Apply color and then reset to default
            return *this;
        }

        LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            stream_ << manip;
            return *this;
        }

    private:
        std::ostream& stream_;
        const std::string color_;
    };

    // Static methods to get instances of LogStream with different colors
    static LogStream red() {
        std::lock_guard<std::mutex> lock(consoleMutex);
        return LogStream(std::cout, ANSI_RED);
    }

    static LogStream green() {
        std::lock_guard<std::mutex> lock(consoleMutex);
        return LogStream(std::cout, ANSI_GREEN);
    }

    static LogStream yellow() {
        std::lock_guard<std::mutex> lock(consoleMutex);
        return LogStream(std::cout, ANSI_YELLOW);
    }

    static LogStream blue() {
        std::lock_guard<std::mutex> lock(consoleMutex);
        return LogStream(std::cout, ANSI_BLUE);
    }

    static LogStream log() {
        std::lock_guard<std::mutex> lock(consoleMutex);
        return LogStream(std::cout, ANSI_RESET);
    }

private:
    static std::mutex consoleMutex;
};

static void test() {
    Console::red() << "This is red text." << std::endl;
    Console::green() << "This is green text." << std::endl;
    Console::yellow() << "This is yellow text." << std::endl;
    Console::blue() << "This is blue text." << std::endl;
    Console::log() << "This is log text." << std::endl;
    std::cout << "This is standard text." << std::endl;
}