#ifndef CREATION_POLICY_HPP
#define CREATION_POLICY_HPP

#include <memory> // for std::unique_ptr, std::shared_ptr
#include <cstdlib> // for malloc/free

namespace dp {

    // Policy for creating objects using new/delete
    template <typename T>
    struct CreateUsingNew {
        static T* Create() {
            return new T();
        }

        static void Destroy(T* p) {
            delete p;
        }
    };

    // Policy for creating objects using malloc/free
    template <typename T>
    struct CreateUsingMalloc {
        static T* Create() {
            void* memory = std::malloc(sizeof(T));
            if (!memory) return nullptr;
            return new(memory) T();
        }

        static void Destroy(T* p) {
            if (p) {
                p->~T();
                std::free(p);
            }
        }
    };

    // Policy for creating objects using std::shared_ptr
    template <typename T>
    struct CreateUsingSharedPtr {
        static T* Create() {
            std::shared_ptr<T>& instance = GetSharedPtr();
            instance = std::make_shared<T>();
            return instance.get();
        }

        static void Destroy(T* p) {
            GetSharedPtr().reset();
        }

    private:
        static std::shared_ptr<T>& GetSharedPtr() {
            static std::shared_ptr<T> instance;
            return instance;
        }
    };

    // Set default creation policy
    template <typename T>
    using DefaultCreationPolicy = CreateUsingNew<T>;

} // namespace dp

#endif // CREATION_POLICY_HPP