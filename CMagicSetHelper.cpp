//
// Created by limo on 2018/7/14.
//

#include "CMagicSetHelper.h"

#include "CMagicSet.h"

namespace limo_ns {

    CMagicSetHelper CMagicSetHelper::create() {
        return CMagicSetHelper();
    }

    std::unique_ptr<CMagicSet>
    CMagicSetHelper::load(std::list<std::string> magic_file_list) {
        this->mp_magic_set = std::unique_ptr<CMagicSet>(new CMagicSet());

        for (auto const &magic_file : magic_file_list) {
            __load_one_file(magic_file);
        }

        return std::move(this->mp_magic_set);
    }

    void CMagicSetHelper::__load_one_file(const std::string &magic_file) {
        // TODO: need implement
    }

}
