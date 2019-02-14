//
// Created by limo on 2018/7/29.
//

#ifndef FILE_CHECK_SINGLETON_H
#define FILE_CHECK_SINGLETON_H


#include <memory>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    virtual ~Singleton() = default;

public:
    static std::shared_ptr<T> Instance() {
        // NOTE: DO NOT use make_share to create the object cause 
        // it's SingleTon<T> instead of make_share is a friend of T
        // ReSharper disable once CppSmartPointerVsMakeFunction
        static auto instance = std::shared_ptr<T>(new T);
        return instance;
    }
};

#endif //FILE_CHECK_SINGLETON_H
