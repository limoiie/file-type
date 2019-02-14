//
// Created by limo on 2/14/2019.
//

#ifndef FILE_TYPE_STRING_UTILS_H
#define FILE_TYPE_STRING_UTILS_H

#include <string>

namespace limo_ns {

    namespace str {

        bool contains(std::string const& sentence, std::string const& tmpl) {
            return std::string::npos != sentence.find(tmpl);
        }

        bool ends_with(std::string const& sentence, std::string const& ext) {
            if (ext.empty()) return true;
            if (sentence.size() < ext.size()) return false;

            auto itr_ext = ext.rbegin();
            auto itr_sentence = sentence.rbegin();
            while (ext.rend() != itr_ext) {
                if (*itr_ext != *itr_sentence) return false;
                ++itr_ext, ++itr_sentence;
            }
            return true;
        }

        std::string cut_back(std::string const& str, size_t const len) {
            return str.substr(0, str.size()-len);
        }

    }

}


#endif //FILE_TYPE_STRING_UTILS_H
