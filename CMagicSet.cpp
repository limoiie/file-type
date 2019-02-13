//
// Created by limo on 2018/7/14.
//

#include "CMagicSet.h"


namespace limo_ns {

    CMagicSet::CMagicSet(std::shared_ptr<SMagicSet> handle)
            : handle_(std::move(handle)){
    }

}
