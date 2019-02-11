//
// Created by limo on 2018/7/17.
//

#ifndef FILE_TYPE_ENCODINGS_H
#define FILE_TYPE_ENCODINGS_H

#include <string>

#include "base.h"

namespace limo_ns {

    namespace encoding {

        enum EFileEncodingType {
            BINARY,
            ASCII,
            UTF7,
            UTF8,
            UTF8_BOM,
            UTF16_LE,
            UTF16_BE,
            ISO8859,
            EXTENDED_ASCII,
            GBK,
            EBCDIC,
            INTERNATIONAL_EBCDIC
        };

        EFileEncodingType check_buffer_encoding(const u_char * i_buffer, size_t i_buffer_size);

    }

}

#endif //FILE_TYPE_ENCODINGS_H
