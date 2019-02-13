//
// Created by limo on 2/13/2019.
//

#include "SMagicSet.h"

namespace limo_ns {

    /*
     * @brief alloc SMagicSet
     *
     * replaced apprentice.c::file_ms_alloc
     */
    SMagicSet *create_magic_set() {
        auto p_magic_set = new SMagicSet();

        if (not magic_set_set_flags(0)) {
            errno = EINVAL;
            p_magic_set = nullptr;
            return nullptr;
        }

        p_magic_set->o.buf = p_magic_set->o.pbuf = nullptr;

        p_magic_set->c.len = 10;
        p_magic_set->c.li = new level_info[p_magic_set->c.len];

        p_magic_set->event_flags = 0;
        p_magic_set->error = -1;

        for (auto &i : p_magic_set->mlist)
            i = nullptr;

        p_magic_set->file = "unknown";
        p_magic_set->line = 0;
        p_magic_set->indir_max = FILE_INDIR_MAX;
        p_magic_set->name_max = FILE_NAME_MAX;
        p_magic_set->elf_shnum_max = FILE_ELF_SHNUM_MAX;
        p_magic_set->elf_phnum_max = FILE_ELF_PHNUM_MAX;
        p_magic_set->elf_notes_max = FILE_ELF_NOTES_MAX;
        p_magic_set->regex_max = FILE_REGEX_MAX;
        p_magic_set->bytes_max = FILE_BYTES_MAX;

        return p_magic_set;
    }

    bool magic_set_set_flags(SMagicSet *p_magic_set, int const flags) {
        if (p_magic_set == nullptr)
            return false;
#if !defined(HAVE_UTIME) && !defined(HAVE_UTIMES)
        if (flags & MAGIC_PRESERVE_ATIME)
            return false;
#endif
        p_magic_set->flags = flags;
        return true;
    }


}