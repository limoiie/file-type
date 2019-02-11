//
// Created by limo on 2018/7/17.
//

#include "encodings.h"

namespace limo_ns {

    namespace encoding {

        static bool looks_ascii(const u_char *i_buff, size_t i_buff_size);
        static bool looks_latin(const u_char *i_buff, size_t i_buff_size);
        static bool looks_extended(const u_char * i_buff, size_t i_buff_size);
        static bool looks_utf8(const u_char *i_buff, size_t i_buff_size);
        static bool looks_utf8_bom(const u_char *i_buff, size_t i_buff_size);
        static bool looks_utf7(const u_char *i_buff, size_t i_buff_size);
        static bool looks_utf16_le(const u_char *i_buff, size_t i_buff_size);
        static bool looks_utf16_be(const u_char *i_buff, size_t i_buff_size);
        static bool looks_gbk(const u_char *i_buff, size_t i_buff_size);

        static void from_ebcdic(u_char *o_out, const u_char *i_buff, size_t i_buff_size);


        EFileEncodingType check_buffer_encoding(const u_char *i_buff, size_t i_buff_size) {
            if (looks_ascii(i_buff, i_buff_size)) {
                if (looks_utf7(i_buff, i_buff_size))
                    return UTF7;
                return ASCII;
            } else if (looks_utf8_bom(i_buff, i_buff_size)) {
                return UTF8_BOM;
            } else if (looks_utf8(i_buff, i_buff_size)) {
                return UTF8;
            } else if (looks_utf16_le(i_buff, i_buff_size)) {
                return UTF16_LE;
            } else if (looks_utf16_be(i_buff, i_buff_size)) {
                return UTF16_BE;
            } else if (looks_latin(i_buff, i_buff_size)) {
                return ISO8859;
            } else if (looks_extended(i_buff, i_buff_size)) {
                return EXTENDED_ASCII;
            } else if (looks_gbk(i_buff, i_buff_size)) {
                return GBK;
            } else {
                u_char out[i_buff_size];
                from_ebcdic(out, i_buff, i_buff_size);

                if (looks_ascii(out, i_buff_size)) {
                    return EBCDIC;
                } else if (looks_latin(out, i_buff_size)) {
                    return INTERNATIONAL_EBCDIC;
                }
            }

            return BINARY;
        }

#define F 0   /* character never appears in text */
#define T 1   /* character appears in plain ASCII text */
#define I 2   /* character appears in ISO-8859 text */
#define X 3   /* character appears in non-ISO extended ASCII (Mac, IBM PC) */

        static char text_chars[256] = {
                /*                  BEL BS HT LF VT FF CR    */
                F, F, F, F, F, F, F, T, T, T, T, T, T, T, F, F,  /* 0x0X */
                /*                              ESC          */
                F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  /* 0x1X */
                T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x2X */
                T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x3X */
                T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x4X */
                T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x5X */
                T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x6X */
                T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  /* 0x7X */
                /*            NEL                            */
                X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  /* 0x8X */
                X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  /* 0x9X */
                I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xaX */
                I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xbX */
                I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xcX */
                I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xdX */
                I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xeX */
                I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   /* 0xfX */
        };

        bool looks_ascii(const u_char *i_buff, size_t const i_buff_size) {
            for (int i = 0; i < i_buff_size; ++i) {
                if (text_chars[i_buff[i]] != T)
                    return false;
            }
            return true;
        }

        bool looks_latin(const u_char *i_buff, size_t const i_buff_size) {
            for (int i = 0; i < i_buff_size; ++i) {
                auto t = text_chars[i_buff[i]];
                if (t != T && i != I)
                    return false;
            }
            return true;
        }

        bool looks_extended(const u_char *i_buff, size_t const i_buff_size) {
            for (int i = 0; i < i_buff_size; ++i) {
                auto t = text_chars[i_buff[i]];
                if (t != T && t != I && t != X)
                    return false;
            }
            return true;
        }

        bool looks_utf8(const u_char *i_buff, size_t const i_buff_size) {
            bool has_weird_ctrl_char = false;
            for (int i = 0; i < i_buff_size; ++i) {
                if ((i_buff[i] & static_cast<u_char>(0x80)) == 0) {          // 0xxxxxxx is plain ASCII
                    if (text_chars[i_buff[i]] != T)
                        has_weird_ctrl_char = true;

                } else if ((i_buff[i] & static_cast<u_char>(0x40)) == 0) {   // 10xxxxxx never 1st byte
                    return false;

                } else {        // 11xxxxxx begins utf-8
                    int following = 0;
                    u_char mask = 0x20;
                    while (mask) {
                        ++following;
                        if ((i_buff[i] & static_cast<u_char>(mask)) == 0)
                            break;
                        mask >>= 1;
                    }

                    // 1111111x never 1st byte
                    if (mask == 1) return false;

                    // the following chars should be 1xxxx0xx
                    for (int j = 0; j < following; ++j) {
                        if (++i >= i_buff_size) break;
                        if ((i_buff[i] & static_cast<u_char>(0x80)) == 0 ||
                            (i_buff[i] & static_cast<u_char>(0x40))) {
                            return false;
                        }
                    }
                }
            }
            return !has_weird_ctrl_char;
        }

        bool looks_utf8_bom(const u_char *i_buff, size_t i_buff_size) {
            if (i_buff_size > 3 &&
                i_buff[0] == 0xef &&
                i_buff[1] == 0xbb &&
                i_buff[2] == 0xbf) {
                // there is bom, check the following text
                return looks_utf8(i_buff+3, i_buff_size-3);
            }
            return false;
        }

        bool looks_utf7(const u_char *i_buff, size_t i_buff_size) {
            if (i_buff_size > 4 &&
                i_buff[0] == '+' &&
                i_buff[1] == '/' &&
                i_buff[2] == 'v') {

                switch (i_buff[3]) {
                    case '8':
                    case '9':
                    case '+':
                    case '/':
                        return true;
                    default:
                        return false;
                }
            }
            return false;
        }

        bool looks_utf16_le(const u_char *i_buff, size_t i_buff_size) {
            if (i_buff[0] == 0xff && i_buff[1] == 0xfe) {
                for (int i = 2; i+1 < i_buff_size; i+=2) {
                    u_int l = i_buff[i];
                    u_int h = i_buff[i+1];

                    auto c = l + (h << (unsigned) 8);
                    if (c == 0xfffe) return false;
                    if (c < 128 && text_chars[c] != T) return false;
                }
                return true;
            }
            return false;
        }

        bool looks_utf16_be(const u_char *i_buff, size_t i_buff_size) {
            if (i_buff[0] == 0xfe && i_buff[1] == 0xff) {
                for (int i = 2; i+1 < i_buff_size; i+=2) {
                    u_int l = i_buff[i+1];
                    u_int h = i_buff[i];

                    auto c = l + (h << (unsigned) 8);
                    if (c == 0xfffe) return false;
                    if (c < 128 && text_chars[c] != T) return false;
                }
                return true;
            }
            return false;
        }

        bool looks_gbk(const u_char *i_buff, size_t i_buff_size) {
            for (int i = 0; i < i_buff_size; ++i) {
                auto byte1 = i_buff[i];
                if (byte1 < 0x80) {
                    // byte 1 in [0x00,0x7f], one byte for one char
                    continue;
                } else if (byte1 > 0x80 && byte1 < 0xff) {
                    // byte 1 in (0x80,0xff), two or four bytes for one char
                    if (++i < i_buff_size) {
                        auto byte2 = i_buff[i];
                        if (byte2 >= 0x40 && byte2 <= 0xfe && byte2 != 0x7f) {
                            // byte2 in [0x40,0xfe] and not equal to 0x7f, two bytes for on char
                        } else if (byte2 >= 0x30 && byte2 <= 0x39) {
                            // byte2 in [0x30,0x39], four bytes for one char
                            // and byte3 should be in [0x81,0xfe]
                            // and byte4 should be in [0x30,0x39]
                            if (++i < i_buff_size) {
                                auto byte3 = i_buff[i];
                                if (byte3 < 0x81 || byte3 > 0xfe)
                                    return false;
                            }

                            if (++i < i_buff_size) {
                                auto byte4 = i_buff[i];
                                if (byte4 < 0x30 || byte4 > 0x39)
                                    return false;
                            }
                        } else
                            return false;
                    }
                }
            }
            return true;
        }

#undef F
#undef T
#undef I
#undef X

        /*
         * This table maps each EBCDIC character to an (8-bit extended) ASCII
         * character, as specified in the rationale for the dd(1) command in
         * draft 11.2 (September, 1991) of the POSIX P1003.2 standard.
         *
         * Unfortunately it does not seem to correspond exactly to any of the
         * five variants of EBCDIC documented in IBM's _Enterprise Systems
         * Architecture/390: Principles of Operation_, SA22-7201-06, Seventh
         * Edition, July, 1999, pp. I-1 - I-4.
         *
         * Fortunately, though, all versions of EBCDIC, including this one, agree
         * on most of the printing characters that also appear in (7-bit) ASCII.
         * Of these, only '|', '!', '~', '^', '[', and ']' are in question at all.
         *
         * Fortunately too, there is general agreement that codes 0x00 through
         * 0x3F represent control characters, 0x41 a nonbreaking space, and the
         * remainder printing characters.
         *
         * This is sufficient to allow us to identify EBCDIC text and to distinguish
         * between old-style and internationalized examples of text.
         */

        u_char ebcdic_to_ascii[] = {
                0,   1,   2,   3,   156,   9, 134, 127, 151, 141, 142,  11,  12,  13,  14,  15,
                16,  17,  18,  19,  157, 133,   8, 135,  24,  25, 146, 143,  28,  29,  30,  31,
                128, 129, 130, 131, 132,  10,  23,  27, 136, 137, 138, 139, 140,   5,   6,   7,
                144, 145, 22,  147, 148, 149, 150,   4, 152, 153, 154, 155,  20,  21, 158,  26,
                ' ', 160, 161, 162, 163, 164, 165, 166, 167, 168, 213, '.', '<', '(', '+', '|',
                '&', 169, 170, 171, 172, 173, 174, 175, 176, 177, '!', '$', '*', ')', ';', '~',
                '-', '/', 178, 179, 180, 181, 182, 183, 184, 185, 203, ',', '%', '_', '>', '?',
                186, 187, 188, 189, 190, 191, 192, 193, 194, '`', ':', '#', '@', '\'','=', '"',
                195, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 196, 197, 198, 199, 200, 201,
                202, 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', '^', 204, 205, 206, 207, 208,
                209, 229, 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 210, 211, 212, '[', 214, 215,
                216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, ']', 230, 231,
                '{', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 232, 233, 234, 235, 236, 237,
                '}', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 238, 239, 240, 241, 242, 243,
                '\\',159, 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 244, 245, 246, 247, 248, 249,
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 250, 251, 252, 253, 254, 255
        };


        void from_ebcdic(u_char *o_out, const u_char *i_buff, size_t i_buff_size) {
            for (auto i = 0; i < i_buff_size; ++i) {
                o_out[i] = ebcdic_to_ascii[i_buff[i]];
            }
        }

    }

}