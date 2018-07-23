//
// Created by limo on 2018/7/14.
//

#include <iostream>
#include "CFileChecker.h"

namespace limo_ns {

    CFileChecker::CFileChecker(const std::shared_ptr<CMagicSet>& p_magic_set_) :
            mp_magic_set(p_magic_set_) {

        std::ostream &os = std::cerr;
        if (os == std::cerr) {

        }
    }

    CFileType CFileChecker::check_file(const std::string &file_name) {
        // first test
        //   fs magic -- file system info
        //   file buffer
        //      file encoding
        //      is tar
        //      is cdf
        //      file soft magic
        //           match
        //               m_set_offset
        //               m_get
        //                   m_copy
        //                   file magic find
        //                   m_convert
        //               magic_check
        //
        //      file asc magic

        // second test

        // third test

        return {};
    }

}
