//
// Created by limo on 2018/7/18.
//


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../encodings.h"


namespace {

    using namespace std;
    using namespace limo_ns::encoding;

    GTEST_TEST(TestEncoding, test_check_type) {  // NOLINT
        vector<string> test_files = {
                R"(D:\Projects\remote\clion\file-type\test\resources\test-utf-8.txt)",
                R"(D:\Projects\remote\clion\file-type\test\resources\test-utf-8-bom.txt)",
                R"(D:\Projects\remote\clion\file-type\test\resources\test-utf-16-be.txt)",
                R"(D:\Projects\remote\clion\file-type\test\resources\test-utf-16-le.txt)",
                R"(D:\Projects\remote\clion\file-type\test\resources\test-gb2312.txt)"
        };

        vector<EFileEncodingType> expect_types = {
                UTF8,
                UTF8_BOM,
                UTF16_BE,
                UTF16_LE,
                GBK
        };

        ASSERT_EQ(test_files.size(), expect_types.size());

        const size_t size = 10;
        u_char buff[size];

        for (auto i = 0; i < test_files.size(); ++i) {
            auto const& test_file = test_files[i];
            auto const expect_type = expect_types[i];
            
            ifstream fs(test_file, ios::in|ios::binary);
            ASSERT_TRUE(fs.is_open());

            fs.read(reinterpret_cast<char*>(buff), size);
            auto const type = check_buffer_encoding(buff, size);
            ASSERT_EQ(type, expect_type);
        }
    }

}
