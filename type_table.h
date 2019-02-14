//
// Created by limo on 2/14/2019.
//

#ifndef FILE_TYPE_CTYPETABLE_H
#define FILE_TYPE_CTYPETABLE_H

#include <string>
#include <map>

#include "Singleton.h"

namespace limo_ns {

    namespace type_table {

        /*
         * @origin apprentice.c::get_type
         */
        int get_type(char const *type_string, char const **cut_type_string);

        /*
         * @origin apprentice.c::get_type
         */
        int get_type_specially(char const *type_string, char const **cut_type_string);

        /*
         * @origin apprentice.c::get_standard_integer_type
         */
        int get_standard_integer_type(char const *type_string, char const **cut_type_string);

        /*
         * @origin apprentice.c::maxoff_t
         */
        off_t max_off_t();


        struct type_tbl_s {
            const char name[16];
            const size_t len;
            const int type;
            const int format;
        };

        class TypeTable : public Singleton<TypeTable> {
            friend class Singleton<TypeTable>;
        private:
            TypeTable();

        public:
            type_tbl_s const *get_by_type(int type);
            char const* get_name_by_type(int type);
            int get_format_by_type(int type);

        private:
            std::map<int, type_tbl_s const*> type_index_;

        };

    }

}


#endif //FILE_TYPE_CTYPETABLE_H
