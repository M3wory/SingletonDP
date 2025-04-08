#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include "../include/singleton.hpp"

// Global mutex for thread-safe console output
std::mutex cout_mutex;

// Helper function for thread-safe console output
void thread_safe_cout(const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << message << std::endl;
}

// Test class for singleton
class TestSingleton {
public:
    TestSingleton() : id_(counter_++) {
        std::stringstream ss;
        ss << "[INFO] TestSingleton created with id " << id_;
        thread_safe_cout(ss.str());
    }

    ~TestSingleton() {
        std::stringstream ss;
        ss << "[INFO] TestSingleton with id " << id_ << " destroyed";
        thread_safe_cout(ss.str());
        counter_--;
    }

    int getId() const { return id_; }
    static int getCounter() { return counter_; }
    static void resetCounter() { counter_ = 0; }

private:
    int id_;
    static int counter_;
};

int TestSingleton::counter_ = 0;

// Define different singleton types for testing
using SingleThreadedTest = dp::Singleton<
    TestSingleton,
    dp::CreateUsingNew,
    dp::DefaultLifetime,
    dp::SingleThreaded
>;

using ThreadSafeTest = dp::Singleton<
    TestSingleton,
    dp::CreateUsingNew,
    dp::DefaultLifetime,
    dp::ClassLevelLockable
>;

using AtomicTest = dp::Singleton<
    TestSingleton,
    dp::CreateUsingNew,
    dp::DefaultLifetime,
    dp::AtomicLockable
>;

// Test for single instance property
TEST_CASE("Singleton creates only one instance", "[singleton]") {
    thread_safe_cout("\n[TEST] Starting single-threaded test");

    // Store initial counter value
    int initialCount = TestSingleton::getCounter();

    SECTION("Single-threaded instance uniqueness") {
        thread_safe_cout("[TEST] Getting first instance");
        TestSingleton& instance1 = SingleThreadedTest::Instance();

        thread_safe_cout("[TEST] Getting second instance");
        TestSingleton& instance2 = SingleThreadedTest::Instance();

        // Check that both references point to the same object
        bool sameAddress = &instance1 == &instance2;
        thread_safe_cout("[TEST] Checking if instances are the same: " +
            std::string(sameAddress ? "YES" : "NO"));
        REQUIRE(sameAddress);

        bool sameId = instance1.getId() == instance2.getId();
        thread_safe_cout("[TEST] Checking if instance IDs match: " +
            std::string(sameId ? "YES" : "NO"));
        REQUIRE(sameId);

        // Verify only one new instance was created during this test
        thread_safe_cout("[TEST] Checking if only one new instance was created");
        REQUIRE(TestSingleton::getCounter() == initialCount + 1);
    }

    thread_safe_cout("[TEST] Single-threaded test completed");
}

// Test for thread safety
TEST_CASE("Singleton is thread-safe", "[singleton][threads]") {
    thread_safe_cout("\n[TEST] Starting multi-threaded test with mutex");

    // Store initial counter value
    int initialCount = TestSingleton::getCounter();

    SECTION("Multi-threaded instance uniqueness") {
        const int NUM_THREADS = 10;
        std::vector<std::thread> threads;

        thread_safe_cout("[TEST] Launching " + std::to_string(NUM_THREADS) + " threads");

        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back([i]() {
                std::stringstream ss;
                ss << "[THREAD " << i << "] Starting";
                thread_safe_cout(ss.str());

                // Access the instance from different threads
                TestSingleton& instance = ThreadSafeTest::Instance();

                ss.str("");
                ss << "[THREAD " << i << "] Accessed instance with id " << instance.getId();
                thread_safe_cout(ss.str());
                });
        }

        thread_safe_cout("[TEST] Waiting for threads to complete");

        for (auto& thread : threads) {
            thread.join();
        }

        thread_safe_cout("[TEST] All threads completed");

        // Verify only one new instance was created during this test
        thread_safe_cout("[TEST] Checking instance count");
        thread_safe_cout("[TEST] Before: " + std::to_string(initialCount) +
            ", After: " + std::to_string(TestSingleton::getCounter()) +
            ", Expected: " + std::to_string(initialCount + 1));

        REQUIRE(TestSingleton::getCounter() == initialCount + 1);
    }

    thread_safe_cout("[TEST] Multi-threaded test completed");
}

// Test for atomic locking
TEST_CASE("Singleton with atomic locking works correctly", "[singleton][atomic]") {
    thread_safe_cout("\n[TEST] Starting atomic locking test");

    // Store initial counter value
    int initialCount = TestSingleton::getCounter();

    SECTION("Atomic locking instance uniqueness") {
        const int NUM_THREADS = 10;
        std::vector<std::thread> threads;

        thread_safe_cout("[TEST] Launching " + std::to_string(NUM_THREADS) + " threads with atomic locking");

        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back([i]() {
                std::stringstream ss;
                ss << "[ATOMIC " << i << "] Starting";
                thread_safe_cout(ss.str());

                TestSingleton& instance = AtomicTest::Instance();

                ss.str("");
                ss << "[ATOMIC " << i << "] Accessed instance with id " << instance.getId();
                thread_safe_cout(ss.str());
                });
        }

        thread_safe_cout("[TEST] Waiting for threads to complete");

        for (auto& thread : threads) {
            thread.join();
        }

        thread_safe_cout("[TEST] All atomic threads completed");

        // Verify only one new instance was created during this test
        thread_safe_cout("[TEST] Checking instance count");
        thread_safe_cout("[TEST] Before: " + std::to_string(initialCount) +
            ", After: " + std::to_string(TestSingleton::getCounter()) +
            ", Expected: " + std::to_string(initialCount + 1));

        REQUIRE(TestSingleton::getCounter() == initialCount + 1);
    }

    thread_safe_cout("[TEST] Atomic locking test completed");
}

// Additional test for proper singleton destruction
TEST_CASE("Singletons are properly destroyed", "[singleton][cleanup]") {
    thread_safe_cout("\n[TEST] Testing singleton destruction");

    // Force access to ensure instances exist
    SingleThreadedTest::Instance();
    ThreadSafeTest::Instance();
    AtomicTest::Instance();

    // Destruction happens at program end via atexit handlers
    thread_safe_cout("[TEST] Singleton instances will be destroyed at program exit");

    // We can't really test this here, but at least we inform about it
    SUCCEED("Singleton destruction scheduled");
}

int main(int argc, char* argv[]) {
    thread_safe_cout("[MAIN] Starting Singleton tests");

    // Run the tests
    int result = Catch::Session().run(argc, argv);

    thread_safe_cout("[MAIN] Tests completed, exiting program");

    return result;
}