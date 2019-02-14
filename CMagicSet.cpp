//
// Created by limo on 2018/7/14.
//

#include <cstdio>

#include "CMagicSet.h"
#include "type_table.h"
#include "logger.h"
#include "utils/string_utils.h"


namespace limo_ns {

#define	EATAB {while (isascii((unsigned char) *l) && \
		      isspace((unsigned char) *l))  ++l;}
#define LOWCASE(l) (isupper((unsigned char) (l)) ? \
			tolower((unsigned char) (l)) : (l))
/*
 * Work around a bug in headers on Digital Unix.
 * At least confirmed for: OSF1 V4.0 878
 */
#if defined(__osf__) && defined(__DECC)
    #ifdef MAP_FAILED
#undef MAP_FAILED
#endif
#endif

#ifndef MAP_FAILED
#define MAP_FAILED (void *) -1
#endif

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#define ALLOC_CHUNK	(size_t)10
#define ALLOC_INCR	(size_t)200

#define MAP_TYPE_USER	0
#define MAP_TYPE_MALLOC	1
#define MAP_TYPE_MMAP	2

    struct magic_entry {
        struct magic *mp;
        uint32_t cont_count;
        uint32_t max_count;
    };

    struct magic_entry_set {
        struct magic_entry *me;
        uint32_t count;
        uint32_t max;
    };

    struct magic_map {
        void *p;
        size_t len;
        int type;
        struct magic *magic[MAGIC_SETS];
        uint32_t nmagic[MAGIC_SETS];
    };


    /*
     * @origin apprentice.c::apprentice_unmap
     */
    static void apprentice_unmap(magic_map *map) {
        if (nullptr == map)
            return;

        switch (map->type) {
            case MAP_TYPE_USER:
                break;
            case MAP_TYPE_MALLOC:
                for (auto &item : map->magic) {
                    void *b = item;
                    void *p = map->p;
                    if (CAST(char *, b) >= CAST(char *, p) &&
                        CAST(char *, b) <= CAST(char *, p) + map->len)
                        continue;
                    free(item);
                }
                free(map->p);
                break;
#ifdef QUICK
            case MAP_TYPE_MMAP:
		if (map->p && map->p != MAP_FAILED)
			(void)munmap(map->p, map->len);
		break;
#endif
            default:
                abort();
        }
        free(map);
    }


    static constexpr auto magicsize = sizeof(magic);
    static const char usg_hdr[] = "cont\toffset\ttype\topcode\tmask\tvalue\tdesc";

    static const char magic_file_ext[] = ".mgc";
    static const char mime_file_ext[] = ".mime";


    static void byteswap(magic *, uint32_t);
    static void bs1(magic *);
    static uint16_t swap2(uint16_t);
    static uint32_t swap4(uint32_t);
    static uint64_t swap8(uint64_t);

    /*
     * Byteswap an mmap'ed file if needed
     */
    static void byteswap(magic *magic, uint32_t nmagic) {
        uint32_t i;
        for (i = 0; i < nmagic; i++)
            bs1(&magic[i]);
    }

    /*
     * swap a short
     */
    static uint16_t swap2(uint16_t sv) {
        uint16_t rv;
        auto s = (uint8_t *)(void *)&sv;
        auto d = (uint8_t *)(void *)&rv;
        d[0] = s[1];
        d[1] = s[0];
        return rv;
    }

    /*
     * swap an int
     */
    static uint32_t swap4(uint32_t sv) {
        uint32_t rv;
        auto s = (uint8_t *)(void *)&sv;
        auto d = (uint8_t *)(void *)&rv;
        d[0] = s[3];
        d[1] = s[2];
        d[2] = s[1];
        d[3] = s[0];
        return rv;
    }

    /*
     * swap a quad
     */
    static uint64_t swap8(uint64_t sv) {
        uint64_t rv;
        auto s = (uint8_t *)(void *)&sv;
        auto d = (uint8_t *)(void *)&rv;
#if 0
        d[0] = s[3];
        d[1] = s[2];
        d[2] = s[1];
        d[3] = s[0];
        d[4] = s[7];
        d[5] = s[6];
        d[6] = s[5];
        d[7] = s[4];
#else
        d[0] = s[7];
        d[1] = s[6];
        d[2] = s[5];
        d[3] = s[4];
        d[4] = s[3];
        d[5] = s[2];
        d[6] = s[1];
        d[7] = s[0];
#endif
        return rv;
    }

    /*
     * byteswap a single magic entry
     */
    static void bs1(magic *m) {
        m->cont_level = swap2(m->cont_level);
        m->offset = swap4((uint32_t)m->offset);
        m->in_offset = swap4((uint32_t)m->in_offset);
        m->lineno = swap4((uint32_t)m->lineno);
        if (IS_STRING(m->type)) {
            m->str_range = swap4(m->str_range);
            m->str_flags = swap4(m->str_flags);
        }
        else {
            m->value.q = swap8(m->value.q);
            m->num_mask = swap8(m->num_mask);
        }
    }


    CMList::CMList(mlist *handle)
            : HandleWrapper(std::unique_ptr<mlist, void(mlist *)>(
                    handle, CMList::free_mlist)) {
    }

    /*
     * @origin apprentice.c::mlist_alloc
     */
    mlist *CMList::alloc_mlist() {
        auto p_mlist = new mlist();
        p_mlist->next = p_mlist->prev = p_mlist;
        return p_mlist;
    }

    /*
     * @origin apprentice.c::mlist_free_one
     */
    void CMList::free_mlist_node(mlist *p_mlist){
        if (p_mlist->map) {
            apprentice_unmap(reinterpret_cast<magic_map *>(p_mlist->map));
        }
        delete p_mlist;
    }

    /*
     * @origin apprentice.c::mlist_free
     */
    void CMList::free_mlist(mlist *p_mlist) {
        if (nullptr == p_mlist)
            return;

        auto curr = p_mlist->next;
        while (curr != p_mlist) {
            auto next = curr->next;
            free_mlist_node(curr);
            curr = next;
        }
        free_mlist_node(p_mlist);
    }

    /*
     * @origin apprentice.c::add_mlist
     */
    bool CMList::insert_front(magic_map *map, size_t idx) {
        auto new_node = CMList::alloc_mlist();

        handle_->map = nullptr;

        new_node->map = idx == 0 ? map : nullptr;
        new_node->magic = map->magic[idx];
        new_node->nmagic = map->nmagic[idx];

        handle_->prev->next = new_node;
        new_node->prev = handle_->prev;
        new_node->next = handle_.get();
        handle_->prev = new_node;

        return true;
    }

    CMagicSet::CMagicSet(magic_set *handle)
            : HandleWrapper(std::unique_ptr<magic_set, void(magic_set *)>(
                    handle, CMagicSet::free_magic_set)) {
    }


    /*
     * @origin apprentice.c::file_ms_alloc
     */
    magic_set *CMagicSet::alloc_magic_set() {
        auto p_magic_set = new magic_set();

        p_magic_set->flags = 0;
        p_magic_set->o.buf = p_magic_set->o.pbuf = nullptr;

        p_magic_set->c.len = 10;
        p_magic_set->c.li = new level_info[p_magic_set->c.len];

        p_magic_set->event_flags = 0;
        p_magic_set->error = -1;

        for (auto& item : p_magic_set->mlist)
            item = nullptr;

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

    /*
     * @origin apprentice.c::file_ms_free
     */
    void CMagicSet::free_magic_set(magic_set *p_magic_set) {
        if (nullptr == p_magic_set) return;

        for (auto& p_mlist : p_magic_set->mlist)
            CMList::free_mlist(p_mlist);

        delete p_magic_set->o.pbuf;
        delete p_magic_set->o.buf;
        delete p_magic_set->c.li;
        delete p_magic_set;
    }

    void CMagicSet::reset() {
        if (handle_->o.buf) {
            delete handle_->o.buf;
            handle_->o.buf = nullptr;
        }
        if (handle_->o.pbuf) {
            delete handle_->o.pbuf;
            handle_->o.pbuf = nullptr;
        }
        handle_->event_flags &= ~EVENT_HAD_ERR;
        handle_->error = -1;
    }

    /*
     * @origin magic.c::magic_setflags
     */
    bool CMagicSet::set_flags(int const flags) {
#if !defined(HAVE_UTIME) && !defined(HAVE_UTIMES)
        if (flags & MAGIC_PRESERVE_ATIME) {
            return false;
        }
#endif
        handle_->flags = flags;
        return true;

    }

    bool CMagicSet::list(std::string const& magic_file) {
        this->reset();
        type_table::TypeTable::Instance();
        this->realloc_mlist();
        load_magic_file(magic_file);
        return false;
    }

    void CMagicSet::realloc_mlist() {
        for (auto &sub_list : handle_->mlist) {
            CMList::free_mlist(sub_list);
            sub_list = CMList::alloc_mlist();
        }
    }

    bool CMagicSet::load_magic_file(std::string const& magic_file) {
        static_assert(FILE_MAGICSIZE == magicsize,
                "magic element size not up to expectation!");



        return true;
    }

    bool CMagicSet::is_loaded() {
        return nullptr != handle_->mlist[0];
    }

    bool CMagicSet::try_map_magic_file(std::string const &magic_file) {
        auto mgc_map = std::unique_ptr<magic_map, void(magic_map*)>(
                new magic_map, apprentice_unmap);
        mgc_map->type = MAP_TYPE_USER;  // unspecified

        auto found_mgc_file = find_magic_file(magic_file);

        auto fd = open(found_mgc_file.c_str(), O_RDONLY | O_BINARY);
        if (-1 == fd) {
            DEF_LOG(error, "cannot open magic file '{}'", found_mgc_file);
            return false;
        }

        struct stat st = {0};
        if (fstat(fd, &st) == -1) {
            DEF_LOG(error, "cannot stat magic file '{}'", found_mgc_file);
            close(fd);
            return false;
        }

        if (st.st_size < 8 || st.st_size > type_table::max_off_t()) {
            DEF_LOG(error, "file '{}' is too {}", found_mgc_file,
                    st.st_size < 8 ? "small" : "large");
            close(fd);
            return false;
        }

        mgc_map->len = static_cast<size_t>(st.st_size);
#ifdef QUICK
        mgc_map->type = MAP_TYPE_MMAP;
        if ((map->p = mmap(0, (size_t)st.st_size, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_FILE, fd, (off_t)0)) == MAP_FAILED) {
            file_error(ms, errno, "cannot map `%s'", found_mgc_file);
            return false;
        }
#else
        mgc_map->type = MAP_TYPE_MALLOC;
        mgc_map->p = static_cast<void *>(new char(mgc_map->len));

        auto const read_size = read(fd, mgc_map->p, static_cast<unsigned int>(mgc_map->len));
        if (read_size != (ssize_t) static_cast<size_t>(mgc_map->len)) {
            DEF_LOG(error, "file '{}' is too {}", found_mgc_file,
                    st.st_size < 8 ? "small" : "large");
            close(fd);
            return false;
        }
#endif
        close(fd);

        if (not check_buffer(mgc_map, found_mgc_file)) {
            return false;
        }
#ifdef QUICK
        if (mprotect(map->p, (size_t)st.st_size, PROT_READ) == -1) {
            file_error(ms, errno, "cannot mprotect `%s'", found_mgc_file);
            return false;
        }
#endif

        return true;
    }

    bool CMagicSet::try_parse_magic_file(std::string const &magic_file) {
        return false;
    }

    std::string CMagicSet::find_magic_file(std::string const &magic_file) {
        auto const mgc_ext = std::string(magic_file_ext);
        auto const mime_ext = std::string(mime_file_ext);

        auto file_name_without_mgc_ext = magic_file;
        if (str::ends_with(magic_file, mgc_ext)) {
            file_name_without_mgc_ext = str::cut_back(magic_file, mgc_ext.size());
        }

        auto found_file = file_name_without_mgc_ext;

        /* Compatibility with old code that looked in .mime */
        if (handle_->flags & MAGIC_MIME) {
            found_file.append(mime_ext);
            if (-1 != access(found_file.c_str(), R_OK)) {
                handle_->flags &= MAGIC_MIME_TYPE;
                return found_file;
            }
        }

        found_file.append(mgc_ext);

        /* Compatibility with old code that looked in .mime */
        if (str::contains(found_file, mime_ext))
            handle_->flags &= MAGIC_MIME_TYPE;

        return found_file;
    }

    bool CMagicSet::check_buffer(magic_map *mgc_map, std::string const &magic_file) {
        uint32_t *ptr;
        uint32_t entries, nentries;
        uint32_t version;

        auto need_byte_swap = false;
        ptr = CAST(uint32_t *, mgc_map->p);
        if (*ptr != MAGICNO) {
            if (swap4(*ptr) != MAGICNO) {
                DEF_LOG(error, "bad magic in '{}'", magic_file);
                return false;
            }
            need_byte_swap = true;
        } else
            need_byte_swap = false;
        if (need_byte_swap)
            version = swap4(ptr[1]);
        else
            version = ptr[1];
        if (version != VERSIONNO) {
            DEF_LOG(error, "File supports only version {} magic files. "
                           "'{}' is version {}", VERSIONNO, magic_file, version);
            return false;
        }

        entries = (uint32_t)(mgc_map->len / sizeof(magic));
        if ((entries * sizeof(magic)) != mgc_map->len) {
            DEF_LOG(error, "Size of '{}' {} is not a multiple of {}",
                    magic_file, mgc_map->len, sizeof(magic));
            return false;
        }

        mgc_map->magic[0] = CAST(magic *, mgc_map->p) + 1;
        nentries = 0;
        for (auto i = 0; i < MAGIC_SETS; ++i) {
            if (need_byte_swap)
                mgc_map->nmagic[i] = swap4(ptr[i + 2]);
            else
                mgc_map->nmagic[i] = ptr[i + 2];
            if (i != MAGIC_SETS - 1)
                mgc_map->magic[i + 1] = mgc_map->magic[i] + mgc_map->nmagic[i];
            nentries += mgc_map->nmagic[i];
        }
        if (entries != nentries + 1) {
            DEF_LOG(error, "Inconsistent entries in '{}' {} != {}",
                    magic_file, entries, nentries+1);
            return false;
        }
        if (need_byte_swap)
            for (auto i = 0; i < MAGIC_SETS; ++i)
                byteswap(mgc_map->magic[i], mgc_map->nmagic[i]);

        return true;
    }

}
