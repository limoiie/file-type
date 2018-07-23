//
// Created by limo on 2018/7/18.
//


#include "../encodings.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace limo_ns::encoding;

int t_main() {
    const size_t size = 10;
    u_char buff[size];

    vector<string> test_files = {
            "../test-utf-8.txt",
            "../test-utf-8-bom.txt",
            "../test-utf-16-be.txt",
            "../test-utf-16-le.txt",
            "../test-gb2312.txt"
    };

    for (const auto &test_file : test_files) {
        ifstream fs(test_file, ios::in|ios::binary);
        if (fs.is_open()) {
            fs.read((char*) buff, size);

            EFileEncodingType type;
            check_buffer_encoding(type, buff, size);
            cout << "type " << type << endl;
        } else {
            cout << "failed to open " << test_file << endl;
        }
    }

    return 0;
}
