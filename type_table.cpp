//
// Created by limo on 2/14/2019.
//

#include <cassert>
#include "type_table.h"

#include "magic_set.h"

namespace limo_ns {

    namespace type_table {


        /*
         * XXX - the actual Single UNIX Specification says that "long" means "long",
         * as in the C data type, but we treat it as meaning "4-byte integer".
         * Given that the OS X version of file 5.04 did the same, I guess that passes
         * the actual test; having "long" be dependent on how big a "long" is on
         * the machine running "file" is silly.
         */
        static const struct type_tbl_s type_tbl[] = {
# define XX(s)		s, (sizeof(s) - 1)
# define XX_NULL	"", 0
                { XX("invalid"),	FILE_INVALID,		FILE_FMT_NONE },
                { XX("byte"),		FILE_BYTE,		    FILE_FMT_NUM },
                { XX("short"),		FILE_SHORT,		    FILE_FMT_NUM },
                { XX("default"),	FILE_DEFAULT,		FILE_FMT_NONE },
                { XX("long"),		FILE_LONG,		    FILE_FMT_NUM },
                { XX("string"),		FILE_STRING,		FILE_FMT_STR },
                { XX("date"),		FILE_DATE,		    FILE_FMT_STR },
                { XX("beshort"),	FILE_BESHORT,		FILE_FMT_NUM },
                { XX("belong"),		FILE_BELONG,		FILE_FMT_NUM },
                { XX("bedate"),		FILE_BEDATE,		FILE_FMT_STR },
                { XX("leshort"),	FILE_LESHORT,		FILE_FMT_NUM },
                { XX("lelong"),		FILE_LELONG,		FILE_FMT_NUM },
                { XX("ledate"),		FILE_LEDATE,		FILE_FMT_STR },
                { XX("pstring"),	FILE_PSTRING,		FILE_FMT_STR },
                { XX("ldate"),		FILE_LDATE,		    FILE_FMT_STR },
                { XX("beldate"),	FILE_BELDATE,		FILE_FMT_STR },
                { XX("leldate"),	FILE_LELDATE,		FILE_FMT_STR },
                { XX("regex"),		FILE_REGEX,		    FILE_FMT_STR },
                { XX("bestring16"),	FILE_BESTRING16,	FILE_FMT_STR },
                { XX("lestring16"),	FILE_LESTRING16,	FILE_FMT_STR },
                { XX("search"),		FILE_SEARCH,		FILE_FMT_STR },
                { XX("medate"),		FILE_MEDATE,		FILE_FMT_STR },
                { XX("meldate"),	FILE_MELDATE,		FILE_FMT_STR },
                { XX("melong"),		FILE_MELONG,		FILE_FMT_NUM },
                { XX("quad"),		FILE_QUAD,		    FILE_FMT_QUAD },
                { XX("lequad"),		FILE_LEQUAD,		FILE_FMT_QUAD },
                { XX("bequad"),		FILE_BEQUAD,		FILE_FMT_QUAD },
                { XX("qdate"),		FILE_QDATE,		    FILE_FMT_STR },
                { XX("leqdate"),	FILE_LEQDATE,		FILE_FMT_STR },
                { XX("beqdate"),	FILE_BEQDATE,		FILE_FMT_STR },
                { XX("qldate"),		FILE_QLDATE,		FILE_FMT_STR },
                { XX("leqldate"),	FILE_LEQLDATE,		FILE_FMT_STR },
                { XX("beqldate"),	FILE_BEQLDATE,		FILE_FMT_STR },
                { XX("float"),		FILE_FLOAT,		    FILE_FMT_FLOAT },
                { XX("befloat"),	FILE_BEFLOAT,		FILE_FMT_FLOAT },
                { XX("lefloat"),	FILE_LEFLOAT,		FILE_FMT_FLOAT },
                { XX("double"),		FILE_DOUBLE,		FILE_FMT_DOUBLE },
                { XX("bedouble"),	FILE_BEDOUBLE,		FILE_FMT_DOUBLE },
                { XX("ledouble"),	FILE_LEDOUBLE,		FILE_FMT_DOUBLE },
                { XX("leid3"),		FILE_LEID3,		    FILE_FMT_NUM },
                { XX("beid3"),		FILE_BEID3,		    FILE_FMT_NUM },
                { XX("indirect"),	FILE_INDIRECT,		FILE_FMT_NUM },
                { XX("qwdate"),		FILE_QWDATE,		FILE_FMT_STR },
                { XX("leqwdate"),	FILE_LEQWDATE,		FILE_FMT_STR },
                { XX("beqwdate"),	FILE_BEQWDATE,		FILE_FMT_STR },
                { XX("name"),		FILE_NAME,		    FILE_FMT_NONE },
                { XX("use"),		FILE_USE,		    FILE_FMT_NONE },
                { XX("clear"),		FILE_CLEAR,		    FILE_FMT_NONE },
                { XX("der"),		FILE_DER,		    FILE_FMT_STR },
                { XX_NULL,		    FILE_INVALID,		FILE_FMT_NONE },
        };

        /*
         * These are not types, and cannot be preceded by "u" to make them
         * unsigned.
         */
        static const struct type_tbl_s special_tbl[] = {
                { XX("der"),		FILE_DER,		FILE_FMT_STR },
                { XX("name"),		FILE_NAME,		FILE_FMT_STR },
                { XX("use"),		FILE_USE,		FILE_FMT_STR },
                { XX_NULL,		    FILE_INVALID,	FILE_FMT_NONE },
        };
# undef XX
# undef XX_NULL

        static int get_type_from_table(type_tbl_s const* table,
                char const *type_string, char const **cut_type_string) {
            const struct type_tbl_s *p;
            for (p = table; p->len; p++) {
                if (strncmp(type_string, p->name, p->len) == 0) {
                    if (cut_type_string)
                        *cut_type_string = type_string + p->len;
                    break;
                }
            }
            return p->type;
        }

        int get_type(char const *type_string, char const **cut_type_string) {
            return get_type_from_table(type_tbl, type_string, cut_type_string);
        }

        int get_type_specially(char const *type_string, char const **cut_type_string) {
            return get_type_from_table(special_tbl, type_string, cut_type_string);
        }

        int get_standard_integer_type(char const *type_string, char const **cut_type_string) {
            int type;

            if (isalpha((unsigned char)type_string[1])) {
                switch (type_string[1]) {
                    case 'C':
                        /* "dC" and "uC" */
                        type = FILE_BYTE;
                        break;
                    case 'S':
                        /* "dS" and "uS" */
                        type = FILE_SHORT;
                        break;
                    case 'I':
                    case 'L':
                        /*
                         * "dI", "dL", "uI", and "uL".
                         *
                         * XXX - the actual Single UNIX Specification says
                         * that "L" means "long", as in the C data type,
                         * but we treat it as meaning "4-byte integer".
                         * Given that the OS X version of file 5.04 did
                         * the same, I guess that passes the actual SUS
                         * validation suite; having "dL" be dependent on
                         * how big a "long" is on the machine running
                         * "file" is silly.
                         */
                        type = FILE_LONG;
                        break;
                    case 'Q':
                        /* "dQ" and "uQ" */
                        type = FILE_QUAD;
                        break;
                    default:
                        /* "d{anything else}", "u{anything else}" */
                        return FILE_INVALID;
                }
                type_string += 2;
            } else if (isdigit((unsigned char)type_string[1])) {
                /*
                 * "d{num}" and "u{num}"; we only support {num} values
                 * of 1, 2, 4, and 8 - the Single UNIX Specification
                 * doesn't say anything about whether arbitrary
                 * values should be supported, but both the Solaris 10
                 * and OS X Mountain Lion versions of file passed the
                 * Single UNIX Specification validation suite, and
                 * neither of them support values bigger than 8 or
                 * non-power-of-2 values.
                 */
                if (isdigit((unsigned char)type_string[2])) {
                    /* Multi-digit, so > 9 */
                    return FILE_INVALID;
                }
                switch (type_string[1]) {
                    case '1':
                        type = FILE_BYTE;
                        break;
                    case '2':
                        type = FILE_SHORT;
                        break;
                    case '4':
                        type = FILE_LONG;
                        break;
                    case '8':
                        type = FILE_QUAD;
                        break;
                    default:
                        /* XXX - what about 3, 5, 6, or 7? */
                        return FILE_INVALID;
                }
                type_string += 2;
            } else {
                /*
                 * "d" or "u" by itself.
                 */
                type = FILE_LONG;
                ++type_string;
            }
            if (cut_type_string)
                *cut_type_string = type_string;
            return type;
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
        static constexpr auto size_off_t = sizeof(off_t);
        static constexpr auto size_int = sizeof(int);
        static constexpr auto size_long = sizeof(long);
#pragma clang diagnostic pop

        off_t max_off_t() {
#if size_off_t == size_int
            return CAST(off_t, INT_MAX);
#elif size_off_t == size_long
            return CAST(off_t, LONG_MAX);
#else
            return 0x7fffffff;
#endif
        }

        TypeTable::TypeTable() {
            type_tbl_s const *p;
            for (p = type_tbl; p->len; p++) {
                assert(p->type < FILE_NAMES_SIZE);
                type_index_[p->type] = p;
            }
            assert(p - type_tbl == FILE_NAMES_SIZE);
        }

        type_tbl_s const *TypeTable::get_by_type(int const type) {
            return type_index_[type];
        }

        char const *TypeTable::get_name_by_type(int const type) {
            return type_index_[type]->name;
        }

        int TypeTable::get_format_by_type(int type) {
            return type_index_[type]->format;
        }

    }

}
