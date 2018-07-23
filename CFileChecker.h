//
// Created by limo on 2018/7/14.
//

#ifndef FILE_TYPE_CFILECHECKER_H
#define FILE_TYPE_CFILECHECKER_H

#include <memory>

#include "CMagicSet.h"
#include "CFileType.h"


namespace limo_ns {

    class CFileChecker {
    public:
        explicit CFileChecker(const std::shared_ptr<CMagicSet>& p_magic_set_);

        CFileType check_file(const std::string &file_name);

    private:
        std::shared_ptr<CMagicSet> mp_magic_set;

    };

}


#endif //FILE_TYPE_CFILECHECKER_H
