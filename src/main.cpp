#include <iostream>
#include <string>
#include <unordered_map>
#include "../include/singleton.hpp"

// Example class for logging
class Logger {
public:
    Logger() { std::cout << "Logger created\n"; }
    ~Logger() { std::cout << "Logger destroyed\n"; }

    void log(const std::string& message) const {
        std::cout << "LOG: " << message << std::endl;
    }
};

// Example class for storing configuration
class Configuration {
public:
    Configuration() { std::cout << "Configuration created\n"; }
    ~Configuration() { std::cout << "Configuration destroyed\n"; }

    void setValue(const std::string& key, const std::string& value) {
        data_[key] = value;
    }

    std::string getValue(const std::string& key) const {
        auto it = data_.find(key);
        return (it != data_.end()) ? it->second : "";
    }

private:
    std::unordered_map<std::string, std::string> data_;
};

// Declare different types of singletons
// 1. Basic logger with default policies
using BasicLogger = dp::Singleton<Logger>;

// 2. Thread-safe configuration using PhoenixSingleton and shared_ptr
using SafeConfig = dp::Singleton<
    Configuration,
    dp::CreateUsingSharedPtr,
    dp::PhoenixSingleton,
    dp::ClassLevelLockable
>;

// 3. Single-threaded logger with no destruction
using PersistentLogger = dp::Singleton<
    Logger,
    dp::CreateUsingNew,
    dp::NoDestroy,
    dp::SingleThreaded
>;

// 4. Thread-local logger
using ThreadLocalLogger = dp::Singleton<
    Logger,
    dp::CreateUsingNew,
    dp::NoDestroy,
    dp::ThreadLocalSingleton
>;

int main() {
    std::cout << "--- Demonstrating Singleton with orthogonal policies ---\n\n";

    // Using the basic logger
    std::cout << "Using BasicLogger:\n";
    BasicLogger::Instance().log("Application started");

    // Using the configuration singleton
    std::cout << "\nUsing SafeConfig:\n";
    SafeConfig::Instance().setValue("server", "localhost");
    SafeConfig::Instance().setValue("port", "8080");

    std::cout << "Server: " << SafeConfig::Instance().getValue("server") << std::endl;
    std::cout << "Port: " << SafeConfig::Instance().getValue("port") << std::endl;

    // Using the persistent logger
    std::cout << "\nUsing PersistentLogger:\n";
    PersistentLogger::Instance().log("Application running");

    // Using the thread-local logger
    std::cout << "\nUsing ThreadLocalLogger:\n";
    ThreadLocalLogger::Instance().log("Message from main thread");

    // Using the basic logger again
    std::cout << "\nUsing BasicLogger again:\n";
    BasicLogger::Instance().log("Application ended");

    std::cout << "\n--- Program terminating ---\n";
    return 0;
}