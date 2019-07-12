// This implementation works on all little endian platforms,
// but will only work on big endian platforms where
// size_t == 64 bits


#ifndef PRC_STRING_STRING_H
#define PRC_STRING_STRING_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
    defined(_M_PPC)

#define PRC_STRING_BIG_ENDIAN

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
    defined(_WIN32)

#define PRC_STRING_LITTLE_ENDIAN

#elif defined()
#error "I don't know what architecture this is!"
#endif

struct ___string_long {
    size_t cap;
    size_t size;
    char* data;
};

#ifdef PRC_STRING_LITTLE_ENDIAN

enum { ___string_long_flag = 0x01 };

#else

enum { ___string_long_flag = 0xF0 };

#endif

enum {
    ___string_min_cap = (sizeof(struct ___string_long) - 1)/sizeof(char) > 2 ?
                        (sizeof(struct ___string_long) - 1)/sizeof(char) : 2
};

#define STRING_MAX (SIZE_MAX >> 1)

struct ___string_short {
    union {
        unsigned char size;
        char lx;
    };
    char data[___string_min_cap];
};

typedef union String {
    struct ___string_long l;
    struct ___string_short s;
} String;

static inline size_t ___string_next_cap(size_t current, size_t desired) {
    if(current >= desired)
        return current;
    current = (size_t)ceil(desired / 16.0) * 16;
    return current < STRING_MAX ? current : STRING_MAX;
}

static inline bool ___string_is_long(String* str) {
    return str->s.size & ___string_long_flag;
}

static inline size_t ___string_short_size(String* str) {
#ifdef PRC_STRING_LITTLE_ENDIAN
    return str->s.size >> 1;
#else
    return str->s.size;
#endif
}

static inline size_t ___string_short_cap(String* str) {
    return ___string_min_cap;
}

static inline size_t ___string_long_size(String* str) {
    return str->l.size;
}

static inline size_t ___string_long_cap(String* str) {
#ifdef PRC_STRING_LITTLE_ENDIAN
    return str->l.cap >> 1;
#else
    return str->l.cap & ~((size_t)___string_long_flag << 56);
#endif
}

static inline void ___string_long_set_cap(String* str, size_t cap) {
#ifdef PRC_STRING_LITTLE_ENDIAN
        str->l.cap = (cap << 1);
#elif
        str->l.cap = cap;
#endif
        str->s.size |= ___string_long_flag;
}

static inline void ___string_long_set_size(String* str, size_t size) {
    str->l.size = size;
}

static inline void ___string_short_set_size(String* str, size_t size) {
#ifdef PRC_STRING_LITTLE_ENDIAN
    str->s.size = (size << 1);
#else
    str->s.size = size;
#endif
}

static inline void string_init(String* str, const char* cstr) {
    size_t len = strlen(cstr);
    if (len < ___string_min_cap) {
        memcpy(str->s.data, cstr, len);
        str->s.data[len] = 0;

        ___string_short_set_size(str, len);
    } else {
        size_t cap = ___string_next_cap(0, len);
        str->l.data = malloc(cap + 1);
        memcpy(str->l.data, cstr, len);
        str->l.data[len] = 0;
        str->l.size = len;
        ___string_long_set_cap(str, cap);
    }
}

static inline void string_init_size(String* str, const char* cstr, size_t len) {
    assert(strlen(cstr) <= len);
    if (len < ___string_min_cap) {
        memcpy(str->s.data, cstr, len);
        str->s.data[len] = 0;

        ___string_short_set_size(str, len);
    } else {
        size_t cap = ___string_next_cap(0, len);
        str->l.data = malloc(cap + 1);
        memcpy(str->l.data, cstr, len);
        str->l.data[len] = 0;
        str->l.size = len;
        ___string_long_set_cap(str, cap);
    }
}

static inline void string_free_resources(String* str) {
    if(___string_is_long(str)) {
        free(str->l.data);
    }
}

static inline const char* string_cstr(String* str) {
    return ___string_is_long(str) ? str->l.data : str->s.data;
}

static inline size_t string_size(String* str) {
    return ___string_is_long(str) ? ___string_long_size(str) : ___string_short_size(str);
}

static inline size_t string_capacity(String* str) {
    return ___string_is_long(str) ? ___string_long_cap(str) : ___string_short_cap(str);
}

static inline char string_get(String* str, size_t index) {
    if(___string_is_long(str)) {
        assert(index < ___string_long_size(str));
        return str->l.data[index];
    } else {
        assert(index < ___string_short_size(str));
        return str->s.data[index];
    }
}

static inline void string_set(String* str, size_t index, char value) {
    if(___string_is_long(str)) {
        assert(index < ___string_long_size(str));
        str->l.data[index] = value;
    } else {
        assert(index < ___string_short_size(str));
        str->s.data[index] = value;
    }
}

static inline bool string_empty(String* str) {
    return ___string_is_long(str) ? (___string_long_size(str) == 0) 
                                  : (___string_short_size(str) == 0);
}

static inline void ___string_long_reserve(String* str, size_t reserve) {
    if(reserve <= ___string_long_cap(str))
        return;

    reserve = ___string_next_cap(0, reserve);
    str->l.data = realloc(str->l.data, reserve + 1);
    str->l.data[reserve] = 0;

    ___string_long_set_cap(str, reserve);
}

// returns true if it had to switch to a large string.
static inline bool ___string_short_reserve(String* str, size_t reserve) {
    if(reserve <= ___string_min_cap)
        return false;

    reserve = ___string_next_cap(0, reserve);
    char* data = malloc(sizeof(char) * reserve + 1);
    memcpy(data, str->s.data, str->s.size + 1);
    data[reserve] = 0;

    str->l.size = str->s.size;
    str->l.data = data;

    ___string_long_set_cap(str, reserve);

    return true;
}

static inline void string_reserve(String* str, size_t reserve) {
    if(___string_is_long(str)) {
        ___string_long_reserve(str, reserve);
    } else {
        ___string_short_reserve(str, reserve);
    }
}

static inline void string_shrink_to_fit(String* str) {
    if(!___string_is_long(str))
        return;

    size_t s = ___string_long_size(str);

    if(s == ___string_long_cap(str))
        return;

    str->l.data = realloc(str->l.data, s + 1);
    str->l.data[s] = 0;
    ___string_long_set_cap(str, s);
}

static inline void string_clear(String* str) {
    if(___string_is_long(str)) {
        str->l.data[0] = 0;
        ___string_long_set_size(str, 0);
    } else {
        str->s.data[0] = 0;
        ___string_short_set_size(str, 0);
    }
}

static inline void ___string_ensure_capacity(String* str, size_t cap) {
    size_t current_size = string_capacity(str);
    cap = ___string_next_cap(current_size, cap);
    string_reserve(str, cap);
}

static inline void ___string_insert_impl(String* str, const char* value, size_t length, size_t index) {
    size_t current_size = string_size(str);
    assert(current_size + length < STRING_MAX);
    ___string_ensure_capacity(str, current_size + length);
    char* data = (char*)string_cstr(str);
    data[current_size + length] = 0;
    if(index != current_size)
        memmove(data + index + length, data + index, current_size - index);
    memmove(data + index, value, length);
}

static inline void string_insert_cstr(String* str, const char* value, size_t index) {
    ___string_insert_impl(str, value, strlen(value), index);
}

static inline void string_insert_string(String* str, String* value, size_t index) {
    ___string_insert_impl(str, string_cstr(value), string_size(value), index);
}

static inline void string_erase(String* str, size_t index, size_t count) {
    size_t current_size = string_size(str);
    assert(index + count < current_size);
    char* data = (char*)string_cstr(str);
    memmove(data + index, data + index + count, current_size - index - count);
}

static inline void string_push_back(String* str, char value) {
    size_t size;

    if(___string_is_long(str)) {
        size = ___string_long_size(str) + 1;
        ___string_long_reserve(str, size);
        ___string_long_set_size(str, size);
    } else {
        size = ___string_short_size(str) + 1;
        if(___string_short_reserve(str, size)) {
            ___string_long_set_size(str, size);
        } else {
            ___string_short_set_size(str, size);
        }
    }
    char* data = (char*)string_cstr(str);
    data[size] = value;
    data[size+1] = 0;
}

static inline void string_pop_back(String* str) {
    size_t size;
    if(___string_is_long(str)) {
        size = ___string_long_size(str);
        if(size == 0)
            return;
        ___string_long_set_size(str, size - 1);
    } else {
        size = ___string_short_size(str);
        if(size == 0)
            return;
        ___string_short_set_size(str, size - 1);
    }
    char* data = (char*)string_cstr(str);
    data[size] = 0;
}

static inline void ___string_append_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    ___string_ensure_capacity(str, size + length);
    char* data = (char*)string_cstr(str);
    memmove(data + size, value, length);
}

static inline void string_append_cstr(String* str, const char* value) {
    ___string_append_impl(str, value, strlen(value));
}

static inline void string_append_string(String* str, String* value) {
    ___string_append_impl(str, string_cstr(value), string_size(value));
}

static inline int ___string_compare_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(size != length)
        return size < length ? -1 : 1;

    return strncmp(string_cstr(str), value, length);
}

static inline int string_compare_cstr(String* str, const char* value) {
    return ___string_compare_impl(str, value, strlen(value));
}

static inline int string_compare_string(String* str, String* value) {
    return ___string_compare_impl(str, string_cstr(value), string_size(value));
}

static inline bool ___string_starts_with_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(length > size)
        return false;

    return strncmp(string_cstr(str), value, length) == 0;
}

static inline bool string_starts_with_cstr(String* str, const char* value) {
    return ___string_starts_with_impl(str, value, strlen(value));
}

static inline bool string_starts_with_string(String* str, String* value) {
    return ___string_starts_with_impl(str, string_cstr(value), string_size(value));
}

static inline bool ___string_ends_with_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(length > size)
        return false;

    return strncmp(string_cstr(str) + (size - length), value, length);
}

static inline bool string_ends_with_cstr(String* str, const char* value) {
    return ___string_ends_with_impl(str, value, strlen(value));
}

static inline bool string_ends_with_string(String* str, String* value) {
    return ___string_ends_with_impl(str, string_cstr(value), string_size(value));
}

static inline void ___string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length) {
    size_t size = string_size(str);
    assert(pos + count < size);
    char* data = (char*)string_cstr(str);
    if(count == length) {
        memmove(data + pos, value, length);
    } else if(count > length) {
        size_t offset = count - length;
        memmove(data + pos + length, data + (size - offset), offset);
        memmove(data + pos, value, length);
        data[size-offset] = 0;
        if(___string_is_long(str))
            ___string_long_set_size(str, size - offset);
        else
            ___string_short_set_size(str, size - offset);
    } else {
        size_t offset = length - count;
        ___string_ensure_capacity(str, size + offset);
        memmove(data + pos + count + offset, data + pos + count, offset);
        memmove(data + pos, value, length);
        data[size+offset] = 0;
        if(___string_is_long(str))
            ___string_long_set_size(str, size + offset);
        else
            ___string_short_set_size(str, size + offset);
    }
}

static inline void string_replace_cstr(String* str, size_t pos, size_t count, const char* value) {
    ___string_replace_impl(str, pos, count, value, strlen(value));
}

static inline void string_replace_string(String* str, size_t pos, size_t count, String* value) {
    ___string_replace_impl(str, pos, count, string_cstr(value), string_size(value));
}

static inline String string_substring(String* str, size_t pos, size_t count) {
    assert(pos + count < string_size(str));
    String result;
    string_init_size(&result, string_cstr(str) + pos, count);
    return result;
}

static inline void string_copy_to(String* str, char* cstr, size_t pos, size_t count) {
    assert(pos + count < string_size(str));
    memmove(cstr, string_cstr(str) + pos, count);
}

static inline void string_resize(String* str, size_t count, char ch) {
    ___string_ensure_capacity(str, count);
    char* data = (char*)string_cstr(str);

    size_t size;

    if(___string_is_long(str)) {
        size = ___string_long_size(str);
        ___string_long_set_size(str, count);
    } else {
        size = ___string_short_size(str);
        ___string_short_set_size(str, count);
    }

    if(size > count) {
        for(size_t i = size; i < count; i++)
            data[i] = ch;
    }

    data[count] = 0;
}

static inline void string_swap(String* left, String* right) {
    String temp = *right;
    memmove(right, left, sizeof(String));
    memmove(left, &temp, sizeof(String));
}

static inline size_t ___string_find_impl(String* str, size_t pos, const char* value) {
    assert(pos < string_size(str));
    char* data = (char*)string_cstr(str);
    char* result = strstr(data + pos, value);
    if(result == NULL)
        return SIZE_MAX;
    return result - data;
}

static inline size_t string_find_cstr(String* str, size_t pos, const char* value) {
    return ___string_find_impl(str, pos, value);
}

static inline size_t string_find_string(String* str, size_t pos, String* value) {
    return ___string_find_impl(str, pos, string_cstr(value));
}

static inline size_t ___string_find_substr_impl(String* str, size_t pos, const char* value, size_t length) {
    size_t size = string_size(str);
    assert(pos < size);
    const char* data = string_cstr(str);
    for(size_t i = pos; i < size - length; i++) {
        if(strncmp(data, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

static inline size_t string_find_substr_cstr(String* str, size_t pos, const char* value, size_t length) {
    assert(length <= strlen(value));
    return ___string_find_substr_impl(str, pos, value, length);
}

static inline size_t string_find_substr_string(String* str, size_t pos, String* value, size_t start, size_t length) {
    assert(start + length <= string_size(value));
    return ___string_find_substr_impl(str, pos, string_cstr(value) + start, length);
}

static inline size_t ___string_rfind_impl(String* str, size_t pos, const char* value, size_t length) {
    assert(pos < string_size(str));
    const char* data = string_cstr(str);
    for(size_t i = pos; i >= 0; --i) {
        if(strncmp(data, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

static inline size_t string_rfind_cstr(String* str, size_t pos, const char* value) {
    return ___string_rfind_impl(str, pos, value, strlen(value));
}

static inline size_t string_rfind_string(String* str, size_t pos, String* value) {
    return ___string_rfind_impl(str, pos, string_cstr(value), string_size(value));
}

static inline size_t string_rfind_substr_cstr(String* str, size_t pos, const char* value, size_t length) {
    assert(length <= strlen(value));
    return ___string_rfind_impl(str, pos, value, length);
}

static inline size_t string_rfind_substr_string(String* str, size_t pos, String* value, size_t start, size_t length) {
    assert(start + length <= string_size(value));
    return ___string_rfind_impl(str, pos, string_cstr(value) + start, length);
}



#endif