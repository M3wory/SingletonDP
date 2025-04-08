#ifndef THREADING_POLICY_HPP
#define THREADING_POLICY_HPP

#include <mutex>
#include <atomic>

namespace dp {

    // Policy for single-threaded access (no synchronization)
    template <typename T>
    class SingleThreaded {
    public:
        class Lock {
        public:
            Lock() {} // Does not lock anything
            ~Lock() {} // Does not unlock anything
        };
    };

    // Policy using mutex for class-level thread safety
    template <typename T>
    class ClassLevelLockable {
    public:
        class Lock {
        private:
            static std::mutex mtx_;
        public:
            Lock() { mtx_.lock(); }
            ~Lock() { mtx_.unlock(); }
        };
    };

    // Static mutex initialization
    template <typename T>
    std::mutex ClassLevelLockable<T>::Lock::mtx_;

    // Policy using atomic operations
    template <typename T>
    class AtomicLockable {
    public:
        class Lock {
        private:
            static std::atomic_flag flag_;
        public:
            Lock() {
                while (flag_.test_and_set(std::memory_order_acquire)) {
                    // Spin-lock
                }
            }

            ~Lock() {
                flag_.clear(std::memory_order_release);
            }
        };
    };

    // Atomic flag initialization
    template <typename T>
    std::atomic_flag AtomicLockable<T>::Lock::flag_ = ATOMIC_FLAG_INIT;

    // Policy using thread_local for thread-specific instances
    template <typename T>
    class ThreadLocalSingleton {
    public:
        class Lock {
        public:
            Lock() {}
            ~Lock() {}
        };

        static T& Instance() {
            static thread_local T* instance = nullptr;
            if (!instance) {
                instance = new T();
            }
            return *instance;
        }
    };

    // Set default threading model
    template <typename T>
    using DefaultThreadingModel = ClassLevelLockable<T>;

} // namespace dp

#endif // THREADING_POLICY_HPP