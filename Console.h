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
    class LogStream {
    public:
        LogStream(std::ostringstream& stream, const std::string& color) : stream_(stream), color_(color) {}

        template <typename T>
        LogStream& operator<<(const T& value) {
            stream_ << color_ << value << ANSI_RESET; // Apply color and then reset to default
            return *this;
        }

        LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            stream_ << manip;
            flush();
            return *this;
        }

    private:
        std::ostringstream& stream_;
        const std::string color_;
    };

    static LogStream red() {
        return LogStream(getBuffer(), ANSI_RED);
    }

    static LogStream green() {
        return LogStream(getBuffer(), ANSI_GREEN);
    }

    static LogStream yellow() {
        return LogStream(getBuffer(), ANSI_YELLOW);
    }

    static LogStream blue() {
        return LogStream(getBuffer(), ANSI_BLUE);
    }

    static LogStream log() {
        return LogStream(getBuffer(), ANSI_RESET);
    }

    static void flush() {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << getBuffer().str();
        getBuffer().str("");
    }

private:
    static std::mutex consoleMutex;
    static std::ostringstream& getBuffer() {
        static thread_local std::ostringstream buffer;
        return buffer;
    }
};

static void test() {
    Console::red() << "This is red text." << std::endl;
    Console::green() << "This is green text." << std::endl;
    Console::yellow() << "This is yellow text." << std::endl;
    Console::blue() << "This is blue text." << std::endl;
    Console::log() << "This is log text." << std::endl;
    std::cout << "This is standard text." << std::endl;
}