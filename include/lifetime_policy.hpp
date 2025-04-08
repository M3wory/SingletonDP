#ifndef LIFETIME_POLICY_HPP
#define LIFETIME_POLICY_HPP

#include <stdexcept>
#include <cstdlib> // for atexit

namespace dp {

    // Policy with standard lifetime (destroyed at program exit)
    template <typename T>
    class DefaultLifetime {
    public:
        static void ScheduleDestruction(void (*pFun)()) {
            std::atexit(pFun);
        }

        static void OnDeadReference() {
            throw std::logic_error("Dead reference to singleton detected");
        }
    };

    // Policy that never destroys the singleton
    template <typename T>
    class NoDestroy {
    public:
        static void ScheduleDestruction(void (*pFun)()) {
            // Do nothing
        }

        static void OnDeadReference() {
            // Do nothing
        }
    };

    // Policy that recreates the singleton when accessed after destruction
    template <typename T>
    class PhoenixSingleton {
    public:
        static void ScheduleDestruction(void (*pFun)()) {
            std::atexit(pFun);
        }

        static void OnDeadReference() {
            // Allow recreation (do nothing special)
        }
    };

    // Set default lifetime policy
    template <typename T>
    using DefaultLifetimePolicy = DefaultLifetime<T>;

} // namespace dp

#endif // LIFETIME_POLICY_HPP