#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <cstdlib> // for atexit
#include "creation_policy.hpp"
#include "threading_policy.hpp"
#include "lifetime_policy.hpp"

namespace dp {

    // Main Singleton template with three orthogonal policies
    template
        <
        typename T,
        template <typename> class CreationPolicy = DefaultCreationPolicy,
        template <typename> class LifetimePolicy = DefaultLifetimePolicy,
        template <typename> class ThreadingModel = DefaultThreadingModel
        >
        class Singleton {
        public:
            // Returns the single instance of the class
            static T& Instance() {
                if (!pInstance_) {
                    typename ThreadingModel<T>::Lock guard;
                    if (!pInstance_) {
                        if (destroyed_) {
                            LifetimePolicy<T>::OnDeadReference();
                            destroyed_ = false;
                        }
                        pInstance_ = CreationPolicy<T>::Create();
                        LifetimePolicy<T>::ScheduleDestruction(&DestroySingleton);
                    }
                }
                return *pInstance_;
            }

        private:
            // Prevent creation, copying and assignment
            Singleton();
            Singleton(const Singleton&);
            Singleton& operator=(const Singleton&);

            // Instance destruction function
            static void DestroySingleton() {
                typename ThreadingModel<T>::Lock guard;
                CreationPolicy<T>::Destroy(pInstance_);
                pInstance_ = nullptr;
                destroyed_ = true;
            }

            // Static class members
            static T* pInstance_;
            static bool destroyed_;
    };

    // Static members initialization
    template
        <
        typename T,
        template <typename> class CreationPolicy,
        template <typename> class LifetimePolicy,
        template <typename> class ThreadingModel
        >
        T* Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::pInstance_ = nullptr;

    template
        <
        typename T,
        template <typename> class CreationPolicy,
        template <typename> class LifetimePolicy,
        template <typename> class ThreadingModel
        >
        bool Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::destroyed_ = false;

} // namespace dp

#endif // SINGLETON_HPP