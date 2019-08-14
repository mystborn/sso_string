/*
    MIT License

    Copyright (c) 2019 Precisamento LLC
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/*
    This file contains definitions and functions for a String data type, 
    built to allocate on the heap as little as possible, without sacrificing 
    any extra memory, at the cost of a little speed. It's implementation 
    closely mirrors parts of libc++'s basic_string<char> type.

    This library is contained in a single header because originally many of 
    the functions were going to be inlined anyways, and the platform specific
    requirements related to integer endianness made this implementation seem
    like the correct choice.

    The type is small enough to pass by value, but you should not do that
    at all if you plan to modify the string's size in any way. If you want
    the string to be mutable, always pass by reference.
*/


#ifndef SSO_STRING_SSO_STRING_H
#define SSO_STRING_SSO_STRING_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
    defined(_M_PPC) || \
    defined(STRING_BIG_ENDIAN)

#define SSO_STRING_BIG_ENDIAN

// This should never be defined unless the compiler defines SIZE_MAX 
// to be different from max(uint32_t) or max(uint64_t), in which case it
// should be the number of bits in size_t - 8.
#ifndef SSO_STRING_SHIFT

// SIZE_MAX == max(uint32_t)
#if SIZE_MAX == 4294967295

#define SSO_STRING_SHIFT 24

// SIZE_MAX == max(uint64_t)
#elif SIZE_MAX == 18446744073709551615

#define SSO_STRING_SHIFT 56

#else

#error "Unable to determine the number of bits in size_t, please define SSO_STRING_SHIFT before including this file as the number of bits in size_t - 8"

#endif
#endif

#elif (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN) || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
    defined(_WIN32) || \
    defined(STRING_LITTLE_ENDIAN)

#define SSO_STRING_LITTLE_ENDIAN

#else
#error "I don't know what architecture this is!"
#endif

struct ___sso_string_long {
    size_t cap;
    size_t size;
    char* data;
};

#ifdef SSO_STRING_LITTLE_ENDIAN

enum { ___sso_string_long_flag = 0x01 };

#else

enum { ___sso_string_long_flag = 0xF0 };

#endif

enum {
    ___sso_string_min_cap = (sizeof(struct ___sso_string_long) - 1)/sizeof(char) > 2 ?
                        (sizeof(struct ___sso_string_long) - 1)/sizeof(char) : 2
};

#define STRING_MAX (SIZE_MAX >> 1)

struct ___sso_string_short {
    union {
        unsigned char size;
        char lx;
    };
    char data[___sso_string_min_cap];
};

typedef union String {
    struct ___sso_string_long l;
    struct ___sso_string_short s;
} String;

static inline size_t ___sso_string_next_cap(size_t current, size_t desired) {
    if(current >= desired)
        return current;
    current = (size_t)max(current * 2, desired);
    return current < STRING_MAX ? current : STRING_MAX;
}

static inline bool ___sso_string_is_long(const String* str) {
    return str->s.size & ___sso_string_long_flag;
}

static inline size_t ___sso_string_short_size(const String* str) {
#ifdef SSO_STRING_LITTLE_ENDIAN
    return str->s.size >> 1;
#else
    return str->s.size;
#endif
}

static inline size_t ___sso_string_short_cap(const String* str) {
    return ___sso_string_min_cap;
}

static inline size_t ___sso_string_long_size(const String* str) {
    return str->l.size;
}

static inline size_t ___sso_string_long_cap(const String* str) {
#ifdef SSO_STRING_LITTLE_ENDIAN
    return str->l.cap >> 1;
#else
    return str->l.cap & ~((size_t)___sso_string_long_flag << SSO_STRING_SHIFT);
#endif
}

static inline void ___sso_string_long_set_cap(String* str, size_t cap) {
#ifdef SSO_STRING_LITTLE_ENDIAN
        str->l.cap = (cap << 1);
#elif
        str->l.cap = cap;
#endif
        str->s.size |= ___sso_string_long_flag;
}

static inline void ___sso_string_long_set_size(String* str, size_t size) {
    str->l.size = size;
}

static inline void ___sso_string_short_set_size(String* str, size_t size) {
#ifdef SSO_STRING_LITTLE_ENDIAN
    str->s.size = (size << 1);
#else
    str->s.size = size;
#endif
}

static inline char* ___sso_string_long_data(const String* str) {
    return str->l.data;
}

static inline char* ___sso_string_short_data(const String* str) {
    return str->s.data;
}

static inline char* ___sso_string_data(const String* str) {
    return ___sso_string_is_long(str) ? ___sso_string_long_data(str) : ___sso_string_short_data(str);
}

/*
    Initializes a string from a c-string.

    str - A pointer to the string to initialize.
    cstr - The contents to initialize the string with. Cannot be NULL.
 */
static inline void string_init(String* str, const char* cstr) {
    size_t len = strlen(cstr);
    if (len < ___sso_string_min_cap) {
        memcpy(str->s.data, cstr, len);
        str->s.data[len] = 0;

        ___sso_string_short_set_size(str, len);
    } else {
        size_t cap = ___sso_string_next_cap(0, len);
        assert((str->l.data = malloc(cap + 1)));
        memcpy(str->l.data, cstr, len);
        str->l.data[len] = 0;
        str->l.size = len;
        ___sso_string_long_set_cap(str, cap);
    }
}

/*
    Initializes a string from a subsection of a c-string.

    str - A pointer to the string to initialize.
    cstr - The contents to initialize the string with. Cannot be NULL.
    len - The number of characters to copy into str.
          Be careful to make sure this does not go past the end of
          the cstr, as that's only checked via assert.
*/

static inline void string_init_size(String* str, const char* cstr, size_t len) {
    assert(len <= strlen(cstr));
    if (len < ___sso_string_min_cap) {
        memcpy(str->s.data, cstr, len);
        str->s.data[len] = 0;

        ___sso_string_short_set_size(str, len);
    } else {
        size_t cap = ___sso_string_next_cap(0, len);
        str->l.data = malloc(cap + 1);
        memcpy(str->l.data, cstr, len);
        str->l.data[len] = 0;
        str->l.size = len;
        ___sso_string_long_set_cap(str, cap);
    }
}

/*
    Creates and initializes a new string value.

    cstr - The contents to initialize the string with. Cannot be NULL.

    return - The initialized String value.
*/
static inline String string_create(const char* cstr) {
    String str;
    string_init(&str, cstr);
    return str;
}

/*
    Allocates and initializes a new string.

    cstr - The contents to initialize the string with. Cannot be NULL.

    return - The initialized String reference. Must be manually freed.
*/
static inline String* string_create_ref(const char* cstr) {
    String* str = malloc(sizeof(String));
    assert(str);
    string_init(str, cstr);
    return str;
}

/*
    Frees any resources used by a string, but does not free
    the string itself.

    str - The string to clean up.
*/
static inline void string_free_resources(String* str) {
    if(___sso_string_is_long(str)) {
        free(str->l.data);
    }
}

/*
    Frees any resources used by a string, then frees the string itself.
*/
static inline void string_free(String* str) {
    string_free_resources(str);
    free(str);
}

/*
    Gets the character data held by a string. This data cannot be altered.
*/
static inline const char* string_cstr(const String* str) {
    return ___sso_string_data(str);
}

/*
    Gets the number of characters in a string, ignoring any terminating characters.
*/
static inline size_t string_size(const String* str) {
    return ___sso_string_is_long(str) ? ___sso_string_long_size(str) : ___sso_string_short_size(str);
}

/*
    Gets the number of characters a string can potential hold without resizing.
*/
static inline size_t string_capacity(const String* str) {
    return ___sso_string_is_long(str) ? ___sso_string_long_cap(str) : ___sso_string_short_cap(str);
}

/*
    Gets the character at the specified index in a string.
*/
static inline char string_get(const String* str, size_t index) {
    if(___sso_string_is_long(str)) {
        // assert(index < ___sso_string_long_size(str));
        return str->l.data[index];
    } else {
        // assert(index < ___sso_string_short_size(str));
        return str->s.data[index];
    }
}

/*
    Sets the character at the specified index in a string.
*/
static inline void string_set(String* str, size_t index, char value) {
    if(___sso_string_is_long(str)) {
        // assert(index < ___sso_string_long_size(str));
        str->l.data[index] = value;
    } else {
        // assert(index < ___sso_string_short_size(str));
        str->s.data[index] = value;
    }
}

/*
    Determines if a string is empty.
*/
static inline bool string_empty(const String* str) {
    return ___sso_string_is_long(str) ? (___sso_string_long_size(str) == 0) 
                                  : (___sso_string_short_size(str) == 0);
}

static inline void ___sso_string_long_reserve(String* str, size_t reserve) {
    if(reserve <= ___sso_string_long_cap(str))
        return;

    reserve = ___sso_string_next_cap(0, reserve);
    str->l.data = realloc(str->l.data, reserve + 1);
    str->l.data[reserve] = 0;

    ___sso_string_long_set_cap(str, reserve);
}

// returns true if it had to switch to a large string.
static inline bool ___sso_string_short_reserve(String* str, size_t reserve) {
    if(reserve < ___sso_string_min_cap)
        return false;

    reserve = ___sso_string_next_cap(0, reserve);
    char* data = malloc(sizeof(char) * (reserve + 1));
    memmove(data, str->s.data, ___sso_string_short_size(str));
    data[reserve] = 0;

    str->l.size = ___sso_string_short_size(str);
    str->l.data = data;

    ___sso_string_long_set_cap(str, reserve);

    return true;
}

/*
    Ensures that a string has a capacity large enough to hold a specified number of characters.

    str - The string to potentially enlarge.
    reserve - The desired minimum capacity.
*/
static inline void string_reserve(String* str, size_t reserve) {
    if(___sso_string_is_long(str)) {
        ___sso_string_long_reserve(str, reserve);
    } else {
        ___sso_string_short_reserve(str, reserve);
    }
}

/*
    Removes any excess memory not being used by a string.
*/
static inline void string_shrink_to_fit(String* str) {
    if(!___sso_string_is_long(str))
        return;

    size_t s = ___sso_string_long_size(str);

    if(s == ___sso_string_long_cap(str))
        return;

    if(s < ___sso_string_min_cap) {
        memmove(str->s.data, str->l.data, s);
        str->s.data[s] = 0;
        // This will clear the long flag.
        ___sso_string_short_set_size(str, s);
    } else {
        str->l.data = realloc(str->l.data, s + 1);
        str->l.data[s] = 0;
        ___sso_string_long_set_cap(str, s);
    }
}

/*
    Clears the contents of a string, but does not free any allocated memory.
*/
static inline void string_clear(String* str) {
    if(___sso_string_is_long(str)) {
        str->l.data[0] = 0;
        ___sso_string_long_set_size(str, 0);
    } else {
        str->s.data[0] = 0;
        ___sso_string_short_set_size(str, 0);
    }
}

static inline void ___sso_string_ensure_capacity(String* str, size_t cap) {
    size_t current_size = string_capacity(str);
    if(cap <= current_size)
        return;
    cap = ___sso_string_next_cap(current_size, cap);
    string_reserve(str, cap);
}

static inline void ___sso_string_insert_impl(String* str, const char* value, size_t length, size_t index) {
    size_t current_size = string_size(str);
    assert(current_size + length < STRING_MAX);
    ___sso_string_ensure_capacity(str, current_size + length);
    char* data = ___sso_string_data(str);
    data[current_size + length] = 0;
    if(index != current_size)
        memmove(data + index + length, data + index, current_size - index);
    memmove(data + index, value, length);
}

/*
    Inserts a c-string into a string at the specified index.
*/
static inline void string_insert_cstr(String* str, const char* value, size_t index) {
    ___sso_string_insert_impl(str, value, strlen(value), index);
}

/*
    Inserts a string into another string at the specified index.
*/
static inline void string_insert_string(String* str, String* value, size_t index) {
    ___sso_string_insert_impl(str, string_cstr(value), string_size(value), index);
}

/*
    Removes a section from a string.
*/
static inline void string_erase(String* str, size_t index, size_t count) {
    size_t current_size = string_size(str);
    assert(index + count <= current_size);
    char* data = ___sso_string_data(str);
    memmove(data + index, data + index + count, current_size - index - count);
    current_size -= count;
    if(___sso_string_is_long(str)) {
        ___sso_string_long_set_size(str, current_size);
        str->l.data[current_size] = 0;
    } else {
        ___sso_string_short_set_size(str, current_size);
        str->s.data[current_size] = 0;
    }
}

/*
    Appends a character to the end of a string.
*/
static inline void string_push_back(String* str, char value) {
    size_t size;
    char* data;

    if(___sso_string_is_long(str)) {
        size = ___sso_string_long_size(str) + 1;
        ___sso_string_long_reserve(str, size);
        ___sso_string_long_set_size(str, size);
        data = str->l.data;
    } else {
        size = ___sso_string_short_size(str) + 1;
        if(___sso_string_short_reserve(str, size)) {
            ___sso_string_long_set_size(str, size);
            data = str->l.data;
        } else {
            ___sso_string_short_set_size(str, size);
            data = str->s.data;
        }
    }
    data[size-1] = value;
    data[size] = 0;
}

/*
    Removes a character from the end of a string and returns
    the characters value.
*/
static inline char string_pop_back(String* str) {
    size_t size;
    if(___sso_string_is_long(str)) {
        size = ___sso_string_long_size(str) - 1;
        if(size == -1)
            return 0;
        ___sso_string_long_set_size(str, size);
    } else {
        size = ___sso_string_short_size(str) - 1;
        if(size == -1)
            return 0;
        ___sso_string_short_set_size(str, size);
    }
    char* data = ___sso_string_data(str);
    char result = data[size];
    data[size] = 0;
    return result;
}

static inline void ___sso_string_append_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    ___sso_string_ensure_capacity(str, size + length);
    char* data = ___sso_string_data(str);
    memmove(data + size, value, length);
    data[size + length] = 0;
}

/*
    Appends a c-string to the end of a string.
*/
static inline void string_append_cstr(String* str, const char* value) {
    ___sso_string_append_impl(str, value, strlen(value));
}

/*
    Appends a string to the end of another string.
*/
static inline void string_append_string(String* str, String* value) {
    ___sso_string_append_impl(str, string_cstr(value), string_size(value));
}

static inline int ___sso_string_compare_impl(const String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(size != length)
        return size < length ? -1 : 1;

    return strncmp(string_cstr(str), value, length);
}

/*
    Compares a string and a c-string in the same fashion as strcmp.
*/
static inline int string_compare_cstr(const String* str, const char* value) {
    return ___sso_string_compare_impl(str, value, strlen(value));
}

/*
    Compares two strings in the same fashion as strcmp.
*/
static inline int string_compare_string(const String* str, String* value) {
    return ___sso_string_compare_impl(str, string_cstr(value), string_size(value));
}

static inline bool ___sso_string_starts_with_impl(const String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(length > size)
        return false;

    return strncmp(string_cstr(str), value, length) == 0;
}

/*
    Determines if a string starts with the characters in a c-string.
*/
static inline bool string_starts_with_cstr(const String* str, const char* value) {
    return ___sso_string_starts_with_impl(str, value, strlen(value));
}

/*
    Determines if a string starts with the characters in another string.
*/
static inline bool string_starts_with_string(const String* str, String* value) {
    return ___sso_string_starts_with_impl(str, string_cstr(value), string_size(value));
}

static inline bool ___sso_string_ends_with_impl(const String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(length > size)
        return false;

    return strncmp(string_cstr(str) + (size - length), value, length) == 0;
}

/*
    Determines if a string ends with the characters in a c-string.
*/
static inline bool string_ends_with_cstr(const String* str, const char* value) {
    return ___sso_string_ends_with_impl(str, value, strlen(value));
}

/*
    Determines if a string ends with the characters in another string.
*/
static inline bool string_ends_with_string(const String* str, String* value) {
    return ___sso_string_ends_with_impl(str, string_cstr(value), string_size(value));
}

static inline void ___sso_string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length) {
    size_t size = string_size(str);
    assert(pos + count <= size);
    char* data;
    if(count == length) {
        data = (char*)string_cstr(str);
        memmove(data + pos, value, length);
    } else if(count > length) {
        data = (char*)string_cstr(str);
        size_t offset = count - length;
        memmove(data + pos + length, data + (size - offset), offset);
        memmove(data + pos, value, length);
        data[size-offset] = 0;
        if(___sso_string_is_long(str))
            ___sso_string_long_set_size(str, size - offset);
        else
            ___sso_string_short_set_size(str, size - offset);
    } else {
        size_t offset = length - count;
        ___sso_string_ensure_capacity(str, size + offset);
        data = (char*)string_cstr(str);
        memmove(data + pos + length, data + pos + count, size-count);
        memmove(data + pos, value, length);
        data[size+offset] = 0;
        if(___sso_string_is_long(str))
            ___sso_string_long_set_size(str, size + offset);
        else
            ___sso_string_short_set_size(str, size + offset);
    }
}

/*
    Replaces a section of a string with the characters in a c-string.
*/
static inline void string_replace_cstr(String* str, size_t pos, size_t count, const char* value) {
    ___sso_string_replace_impl(str, pos, count, value, strlen(value));
}

/*
    Replaces a section of a string with the characters in another string.
*/
static inline void string_replace_string(String* str, size_t pos, size_t count, String* value) {
    ___sso_string_replace_impl(str, pos, count, string_cstr(value), string_size(value));
}

/*
    Initializes a string with the data from a slice of another string.
*/
static inline void string_substring(const String* str, size_t pos, size_t count, String* value) {
    assert(pos + count <= string_size(str));
    string_init_size(value, string_cstr(str) + pos, count);
}

/*
    Copies the data from a slice of a string into a c-string, overwriting any 
    previous data. Does not add a terminating character at the end.
*/
static inline void string_copy_to(const String* str, char* cstr, size_t pos, size_t count) {
    assert(pos + count <= string_size(str));
    memmove(cstr, string_cstr(str) + pos, count);
}

/*
    Resizes a string, adding the specified character to fill any new spots. 
    Removes trailing characters if the new size is smaller than the current 
    size.
*/
static inline void string_resize(String* str, size_t count, char ch) {
    assert(count < STRING_MAX);
    ___sso_string_ensure_capacity(str, count);
    char* data = ___sso_string_data(str);

    size_t size;

    if(___sso_string_is_long(str)) {
        size = ___sso_string_long_size(str);
        ___sso_string_long_set_size(str, count);
    } else {
        size = ___sso_string_short_size(str);
        ___sso_string_short_set_size(str, count);
    }
    
    for(size_t i = size; i < count; i++)
        data[i] = ch;

    data[count] = 0;
}

/*
    Swaps the contents of two strings.
*/
static inline void string_swap(String* left, String* right) {
    String temp = *right;
    memmove(right, left, sizeof(String));
    memmove(left, &temp, sizeof(String));
}
static inline size_t ___sso_string_find_impl(const String* str, size_t pos, const char* value) {
    assert(pos < string_size(str));
    char* data = ___sso_string_data(str);
    char* result = strstr(data + pos, value);
    if(result == NULL)
        return SIZE_MAX;
    return result - data;
}

/*
    Finds the starting index of the first occurrence of a c-string in a string. 
    Returns SIZE_MAX if the c-string could not be found.
*/
static inline size_t string_find_cstr(const String* str, size_t pos, const char* value) {
    return ___sso_string_find_impl(str, pos, value);
}

/*
    Finds the starting index of the first occurrence of a string in another 
    string. Returns SIZE_MAX if the string could not be found.
*/
static inline size_t string_find_string(const String* str, size_t pos, String* value) {
    return ___sso_string_find_impl(str, pos, string_cstr(value));
}

static inline size_t ___sso_string_find_substr_impl(const String* str, size_t pos, const char* value, size_t length) {
    size_t size = string_size(str) - length;
    assert(pos < size);
    const char* data = string_cstr(str);
    for(size_t i = pos; i < size; i++) {
        if(strncmp(data + i, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

/*
    Finds the starting index of the first occurrence of part of a c-string in 
    a string. Returns SIZE_MAX if the section could not be found.
*/
static inline size_t string_find_substr_cstr(const String* str, size_t pos, const char* value, size_t length) {
    assert(length <= strlen(value));
    return ___sso_string_find_substr_impl(str, pos, value, length);
}

/*
    Finds the starting index of the first occurrence of part of a string in 
    another string. Returns SIZE_MAX if the section could not be found.
*/
static inline size_t string_find_substr_string(const String* str, size_t pos, String* value, size_t start, size_t length) {
    assert(start + length <= string_size(value));
    return ___sso_string_find_substr_impl(str, pos, string_cstr(value) + start, length);
}

static inline size_t ___sso_string_rfind_impl(const String* str, size_t pos, const char* value, size_t length) {
    assert(pos < string_size(str));
    const char* data = string_cstr(str);
    size_t i = pos + 1;
    
    while(i != 0) {
        if(strncmp(data + --i, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

/*
    Finds the starting index of the last occurrence of a c-string in a string.
    Returns SIZE_MAX if the c-string could not be found.
*/
static inline size_t string_rfind_cstr(const String* str, size_t pos, const char* value) {
    return ___sso_string_rfind_impl(str, pos, value, strlen(value));
}

/*
    Finds the starting index of the last occurrence of a string in another 
    string. Returns SIZE_MAX if the c-string could not be found.
*/
static inline size_t string_rfind_string(const String* str, size_t pos, String* value) {
    return ___sso_string_rfind_impl(str, pos, string_cstr(value), string_size(value));
}

/*
    Finds the starting index of the last occurrence of part of a c-string in
    a string. Returns SIZE_MAX if the section could not be found.
*/
static inline size_t string_rfind_substr_cstr(const String* str, size_t pos, const char* value, size_t length) {
    assert(length <= strlen(value));
    return ___sso_string_rfind_impl(str, pos, value, length);
}

/*
    Finds the starting index of the last occurrence of part of a string in
    another string. Returns SIZE_MAX if the section could not be found.
*/
static inline size_t string_rfind_substr_string(const String* str, size_t pos, String* value, size_t start, size_t length) {
    assert(start + length <= string_size(value));
    return ___sso_string_rfind_impl(str, pos, string_cstr(value) + start, length);
}

// If C11 is available, use the _Generic macro to select the correct
// string function, otherwise just default to using cstrings.

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define string_insert(str, value, index) _Generic((value), char*: string_insert_cstr, String*: string_insert_string)((str), (value), (index))
#define string_append(str, value) _Generic((value), char*: string_append_cstr, String*: string_append_string)((str), (value))
#define string_compare(str, value) _Generic((value), char*: string_compare_cstr, String*: string_compare_string)((str), (value))
#define string_starts_with(str, value) _Generic((value), char*: string_starts_with_cstr, String*: string_starts_with_string)((str), (value))
#define string_ends_with(str, value) _Generic((value), char*: string_ends_with_cstr, String*: string_ends_with_string)((str), (value))
#define string_replace(str, pos, count, value) _Generic((value), char*: string_replace_cstr, String*: string_replace_string)((str), (pos), (count), (value))
#define string_find(str, pos, value) _Generic((value), char*: string_find_cstr, String*: string_find_string)((str), (pos), (value))
#define string_rfind(str, pos, value) _Generic((value), char*: string_rfind_cstr, String*: string_rfind_string)((str), (pos), (value))

#else

#define string_insert(str, value, index) string_insert_cstr(str, value, index)
#define string_append(str, value) string_append_cstr(str, value)
#define string_compare(str, value) string_compare_cstr(str, value)
#define string_starts_with(str, value) string_starts_with_cstr(str, value)
#define string_ends_with(str, value) string_ends_with_cstr(str, value)
#define string_replace(str, pos, count, value) string_replace_cstr(str, pos, count, value)
#define string_find(str, pos, value) string_find_cstr(str, pos, value)
#define string_rfind(str, pos, value) string_rfind_cstr(str, pos, value)

#endif // C11

#endif