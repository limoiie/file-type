//
// Created by limo on 2/14/2019.
//

#ifndef FILE_TYPE_HANDLEWRAPPER_H
#define FILE_TYPE_HANDLEWRAPPER_H

#include <memory>

template <typename T>
class HandleWrapper {
public:
    explicit HandleWrapper(std::unique_ptr<T>&& handle)
            : handle_(std::move(handle)){
    }

    virtual ~HandleWrapper() = default;

    T *release() {
        return handle_.release();
    }

    void reset(std::unique_ptr<T>&& handle) {
        handle_ = handle;
    }

protected:
    std::unique_ptr<T> handle_;

};

#endif //FILE_TYPE_HANDLEWRAPPER_H
