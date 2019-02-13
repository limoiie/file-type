//
// Created by limo on 2018/7/14.
//

#ifndef FILE_TYPE_CMAGICSET_H
#define FILE_TYPE_CMAGICSET_H

#include <memory>

#include "SMagicSet.h"


namespace limo_ns {

    class CMagicSet {
        friend class CMagicSetHelper;

    private:
        explicit CMagicSet(std::shared_ptr<SMagicSet> handle);

        static std::shared_ptr<SMagicSet> build_handle();

    private:
        std::shared_ptr<SMagicSet> handle_;

    };

}

#endif //FILE_TYPE_CMAGICSET_H
