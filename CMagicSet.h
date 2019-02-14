//
// Created by limo on 2018/7/14.
//

#ifndef FILE_TYPE_CMAGICSET_H
#define FILE_TYPE_CMAGICSET_H

#include <memory>

#include "magic_set.h"
#include "magic.h"
#include "HandleWrapper.h"


namespace limo_ns {

    struct magic_map;

    class CMList : public HandleWrapper<mlist> {
    public:
        explicit CMList(mlist *handle);

        static mlist *alloc_mlist();
        static void free_mlist_node(mlist *p_mlist);
        static void free_mlist(mlist *p_mlist);

        bool insert_front(magic_map *map, size_t idx);

    };

    class CMagicSet : public HandleWrapper<magic_set> {
        friend class CMagicSetHelper;

    private:
        explicit CMagicSet(magic_set *handle);

        void realloc_mlist();
        bool check_buffer(magic_map *mgc_map, std::string const& magic_file);
        std::string find_magic_file(std::string const& magic_file);
        bool try_map_magic_file(std::string const& magic_file);
        bool try_parse_magic_file(std::string const& magic_file);

    public:
        static magic_set *alloc_magic_set();
        static void free_magic_set(magic_set *p_magic_set);

        void reset();
        bool set_flags(int flags);
        bool list(std::string const& magic_file);

        bool load_magic_file(std::string const& magic_file);
        bool is_loaded();

    };

}

#endif //FILE_TYPE_CMAGICSET_H
