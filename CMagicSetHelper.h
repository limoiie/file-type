//
// Created by limo on 2018/7/14.
//

#ifndef FILE_TYPE_CMAGICSETHELPER_H
#define FILE_TYPE_CMAGICSETHELPER_H

#include <memory>
#include <list>

#include "CMagicSet.h"

namespace limo_ns {

    class CMagicSetHelper {
    public:
        static CMagicSetHelper create();

        std::unique_ptr<CMagicSet> load(std::list<std::string> magic_file_list);

    private:
        void __load_one_file(const std::string &magic_file);

        std::unique_ptr<CMagicSet> mp_magic_set;

    };

}

#endif //FILE_TYPE_CMAGICSETHELPER_H
