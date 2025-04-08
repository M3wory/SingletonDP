# Singleton Design Pattern with Orthogonal Policies

This project implements the Singleton design pattern using the orthogonal policy-based design approach described in Andrei Alexandrescu's book "Modern C++ Design" and implemented in his Loki library.

## Overview

The Singleton pattern ensures a class has only one instance and provides a global access point to that instance. This implementation extends the basic pattern with a flexible, template-based approach that separates concerns into orthogonal policies:

- **Creation Policy** - How the singleton instance is created and destroyed
- **Lifetime Policy** - When the instance is destroyed and how to handle access after destruction
- **Threading Policy** - How to ensure thread-safety in multithreaded environments

By mixing and matching these policies, you can create highly customized singleton implementations without modifying the core code.

## Features

- **Policy-Based Design**: Create custom singletons by combining independent policies
- **Thread Safety**: Multiple synchronization strategies (mutex, atomic, thread-local)
- **Lifetime Management**: Control whether instances persist for the program lifetime or can be recreated
- **Memory Management**: Different allocation strategies (new/delete, malloc/free, shared_ptr)
- **Double-Checked Locking**: Efficient thread-safe initialization

## Project Structure

```
singleton-project/
├── CMakeLists.txt            # Main CMake configuration
├── include/                  # Header files directory
│   ├── singleton.hpp         # Main Singleton implementation
│   ├── creation_policy.hpp   # Instance creation strategies
│   ├── threading_policy.hpp  # Thread synchronization strategies
│   └── lifetime_policy.hpp   # Lifetime management strategies
├── src/                      # Source files
│   └── main.cpp              # Usage examples
└── tests/                    # Tests directory
    └── test.cpp              # Catch2-based tests
```

## Policies

### Creation Policies

- `CreateUsingNew`: Standard heap allocation with new/delete
- `CreateUsingMalloc`: C-style allocation with malloc/free
- `CreateUsingSharedPtr`: Smart pointer management with std::shared_ptr

### Lifetime Policies

- `DefaultLifetime`: Schedules destruction at program exit
- `NoDestroy`: Never destroys the singleton instance
- `PhoenixSingleton`: Allows recreation after destruction

### Threading Policies

- `SingleThreaded`: No synchronization (for single-threaded applications)
- `ClassLevelLockable`: Thread synchronization with std::mutex
- `AtomicLockable`: Thread synchronization with std::atomic_flag
- `ThreadLocalSingleton`: Thread-specific instances

## Building and Running

### Prerequisites

- C++17 compatible compiler
- CMake 3.14 or higher
- Git (for fetching Catch2)

### Build Steps

```bash
# Clone the repository
git clone https://github.com/yourusername/singleton-project.git
cd singleton-project

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run the demo
./singleton_demo

# Run the tests
./singleton_tests
```

## Usage Examples

### Basic Usage

```cpp
// Define a singleton with default policies
using MyLoggerSingleton = dp::Singleton<Logger>;

// Access the singleton instance
Logger& logger = MyLoggerSingleton::Instance();
logger.log("Application started");
```

### Custom Singleton

```cpp
// Thread-safe configuration singleton that can be recreated after destruction
using ConfigSingleton = dp::Singleton<
    Configuration,                  // The class to make a singleton
    dp::CreateUsingSharedPtr,       // Use shared_ptr for memory management
    dp::PhoenixSingleton,           // Allow recreation after destruction
    dp::ClassLevelLockable          // Thread-safe with mutex
>;

// Usage
Configuration& config = ConfigSingleton::Instance();
config.setValue("server", "localhost");
```

### Thread-Local Singleton

```cpp
// Create a thread-local logger
using ThreadLogger = dp::Singleton<
    Logger,
    dp::CreateUsingNew,
    dp::NoDestroy,
    dp::ThreadLocalSingleton        // Each thread gets its own instance
>;

// Usage in thread
ThreadLogger::Instance().log("Message from thread");
```

## Further Reading

- Alexandrescu, A. (2001). *Modern C++ Design: Generic Programming and Design Patterns Applied*. Addison-Wesley.
- [Loki Library](http://loki-lib.sourceforge.net/) - Reference implementation by Alexandrescu

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Andrei Alexandrescu for the original design and implementation in "Modern C++ Design"
- The Catch2 team for the testing framework
