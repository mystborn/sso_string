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

    The type is small enough to pass by value, but you should not do that
    at all if you plan to modify the string in any way.

    Essentially all bounds checking is done using assert, so if that is something desired
    in a production environment, it will have to be implemented on the callers side.
*/


#ifndef SSO_STRING_SSO_STRING_H
#define SSO_STRING_SSO_STRING_H

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

#ifdef SSO_STRING_BUILD
    #if defined(_WIN32)
        #define SSO_STRING_EXPORT __declspec(dllexport)
    #elif defined(__ELF__)
        #define SSO_STRING_EXPORT __attribute__((visibility ("default")))
    #else
        #define SSO_STRING_EXPORT
    #endif
#else
    #if defined(_WIN32)
        #define SSO_STRING_EXPORT __declspec(dllimport)
    #else
        #define SSO_STRING_EXPORT
    #endif
#endif

#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
    defined(_M_PPC) || \
    defined(STRING_BIG_ENDIAN)

#define SSO_STRING_BIG_ENDIAN

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

#ifndef SSO_STRING_ASSERT_ARG
#define SSO_STRING_ASSERT_ARG assert
#endif

#ifndef SSO_STRING_ASSERT_BOUNDS
#define SSO_STRING_ASSERT_BOUNDS assert
#endif

struct sso_string_long {
    size_t cap;
    size_t size;
    char* data;
};

#ifdef SSO_STRING_LITTLE_ENDIAN

enum { SSO_STRING_LONG_FLAG = 0x01 };

#else

enum { SSO_STRING_LONG_FLAG = 0xF0 };

#endif

enum {
    SSO_STRING_MIN_CAP = ((sizeof(struct sso_string_long) - 1)/sizeof(char) > 2 ?
                        (sizeof(struct sso_string_long) - 1)/sizeof(char) : 2) - 1
};

#define STRING_MAX (SIZE_MAX >> 1)

struct sso_string_short {
    union {
        unsigned char size;
        char lx;
    };
    char data[SSO_STRING_MIN_CAP + 1];
};

typedef union String {
    struct sso_string_long l;
    struct sso_string_short s;
} String;

/**
    A numeric representation of a unicode character/codepoint.
 */
typedef uint32_t Char32;

/**
    Initializes a string from a c-string.

    @param str A pointer to the string to initialize.
    @param cstr The contents to initialize the string with.

    @return true on success, false on allocation failure.
 */
SSO_STRING_EXPORT bool string_init(String* str, const char* cstr);

/**
    Initializes a string from a subsection of a c-string.

    @param str A pointer to the string to initialize.
    @param cstr The contents to initialize the string with.
    @param len The number of characters to copy into str.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_init_size(String* str, const char* cstr, size_t length);

/**
    Creates and initializes a new string value.

    @param cstr The contents to initialize the string with.

    @return The initialized String value.

    @remarks There is no way to check for allocation failure.
*/
static inline String string_create(const char* cstr);

/**
    Allocates and initializes a new string.

    @param cstr The contents to initialize the string with.

    @return The initialized String reference. Must be manually freed. 
            NULL on allocation failure.
*/
static inline String* string_create_ref(const char* cstr);

/**
    Frees any resources used by a string, but does not free 
    the string itself.

    @param str The string to clean up.
*/
static inline void string_free_resources(String* str);

/**
    Frees any resources used by a string, then frees the string itself.

    @param str The string to clean up.
*/
static inline void string_free(String* str);

/**
    Gets the character data held by a string. This data cannot be altered.

    @param str The string to get the internal representation of.

    @return The internal representation of the string as a c-string.
*/
static inline const char* string_data(const String* str);

/**
    Gets the character data held by a string. This data should be altered carefully.

    @param str The string to get the internal representation of.

    @return The internal representation of the string as a c-string.
*/
static inline char* string_cstr(String* str);

/**
    Gets the number of bytes in a string, ignoring any terminating characters.

    @param str The string to get the size of.
    
    @return The number of bytes on the string.
*/
static inline size_t string_size(const String* str);

/**
    Gets the number of codepoints in a string.

    @param str The string to get the codepoint count of.

    @return The number of codepoints in the string.
*/
SSO_STRING_EXPORT size_t string_u8_codepoints(const String* str);

/**
    Gets the number of characters a string can potential hold without resizing.
    This does NOT include the NULL terminating character.

    @param str The string to get the capacity of.

    @return The internal capacity of the string.
*/
static inline size_t string_capacity(const String* str);

/**
    Gets the character at the specified index in a string.

    @param str The string to get the character from.
    @param index The index of the character to get.

    @return The character at the specified index.
*/
static inline char string_get(const String* str, size_t index);

/**
    Gets the unicode character at the specified byte index.

    @param str The string to get the unicode character from.
    @param index The starting byte index of the unicode character.

    @return Thee unicode character starting at the specified byte index.
*/
SSO_STRING_EXPORT Char32 string_u8_get(const String* str, size_t index);

/**
    Gets the unicode character at the specified byte index, 
    optionally getting the number of bytes that the character
    takes in the string.

    @param str The string to get the unicode character from.
    @param index The starting byte index of the unicode character.
    @param out_size If not NULL, contains the number of bytes used to represent unicode character.

    @return Thee unicode character starting at the specified byte index.

    @remark This function can be used to easily iterate over a UTF-8 string.
*/
SSO_STRING_EXPORT Char32 string_u8_get_with_size(const String* str, size_t index, int* out_size);

/**
    Gets the unicode character size at the specified byte index in bytes. 

    @param str The string to gert the unicode character size from.
    @param index The starting byte index of the unicode character.

    @return The number of bytes used to represent the unicode character.
*/
SSO_STRING_EXPORT int string_u8_codepoint_size(const String* str, size_t index);

/**
    Sets the character at the specified index in a string.

    @param str The string to modify.
    @param index The index of the character to replace.
    @param value The new character to replace the existing character with.
*/
static inline void string_set(String* str, size_t index, char value);

/**
    Replaces the unicode character at the specified index in a string.

    @param str The string to modify.
    @param index The index of the character to replace.
    @param value The new character to replace the existing character with.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_u8_set(String* str, size_t index, Char32 value);

/**
    Determines if a string has no characters.

    @return true if the string is empty; false otherwise.
*/
static inline bool string_empty(const String* str);

/**
    Determines if the string is NULL or has no characters.

    @return true if the string is NULL or empty; false otherwise.
*/
static inline bool string_is_null_or_empty(const String* str);

/**
    Working with ASCII or UTF-8 strings, determines if the string is NULL,
    empty, or comprised of only whitespace characters.

    @return true if the string is NULL, empty, or comprised only of whitespace characters; false otherwise.
*/
SSO_STRING_EXPORT bool string_u8_is_null_or_whitespace(const String* str);

/**
    Ensures that a string has a capacity large enough to hold a specified number of characters. 
    Does not include any terminating characters.

    @param str - The string to potentially enlarge.
    @param reserve - The desired minimum capacity, not including the NULL terminating character.

    @return true on success, false on allocation failure.
*/
static inline bool string_reserve(String* str, size_t reserve);

/**
    Removes any excess memory not being used by a string.

    @param str The string to shrink.
*/
SSO_STRING_EXPORT void string_shrink_to_fit(String* str);

/**
    Clears the contents of a string, but does not free any allocated memory.

    @param str The string to clear.

    @see string_shrink_to_fit
*/
static inline void string_clear(String* str);

/**
    Inserts a c-string into a string at the specified index.

    @param str The string to insert into.
    @param value The c-string to insert.
    @param index The index of the string to insert the value into.

    @return true on success, false on allocation failure.
*/
static inline bool string_insert_cstr(String* str, const char* value, size_t index);

/**
    Inserts a string into another string at the specified index.

    @param str The string to insert into.
    @param value The string to insert.
    @param index The index of the string to insert the value into.

    @return true on success, false on allocation failure.
*/
static inline bool string_insert_string(String* str, const String* value, size_t index);

/**
    Inserts a section of a c-string into a string at the specified index.

    @param str The string to insert into.
    @param value The c-string to insert.
    @param index The index of the string to insert the value into.
    @param start The starting index of the value to insert.
    @param count The number of characters following start to insert.

    @return true on success, false on allocation failure.
*/
static inline bool string_insert_cstr_part(String* str, const char* value, size_t index, size_t start, size_t count);

/**
    Inserts a section of a string into another string at the specified index.

    @param str The string to insert into.
    @param value The string to insert.
    @param index The index of the string to insert the value into.
    @param start The starting index of the value to insert.
    @param count The number of characters following start to insert.

    @return true on success, false on allocation failure.
*/
static inline bool string_insert_string_part(String* str, const String* value, size_t index, size_t start, size_t count);

/**
    Removes a section from a string.

    @param str The string to modify.
    @param index The starting index of the characters to erase.
    @param count The number of characters following index to erase.
*/
SSO_STRING_EXPORT void string_erase(String* str, size_t index, size_t count);

/**
    Appends a character to the end of a string.

    @param str The string to append to.
    @param value The character to append.

    @return true on success, false on allocation failure.

*/
SSO_STRING_EXPORT bool string_push_back(String* str, char value);

/**
    Appends a unicode character to the end of a string.

    @param str The string to append to.
    @param value The unicode character to append.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_u8_push_back(String* str, Char32 value);

/**
    Removes a character from the end of a string and returns
    the characters value. 

    @param str The string to get the last character of.

    @return The last character of the string if any, '\\0' otherwise.
*/
SSO_STRING_EXPORT char string_pop_back(String* str);

/**
    Removes a unicode character from the end of a string and
    returns the characters value.

    @param str The string to get the last unicode character of.

    @return The last unicode character of the string if any, '\\0' otherwise.
*/
SSO_STRING_EXPORT Char32 string_u8_pop_back(String* str);

/**
    Appends a c-string to the end of a string.

    @param str The string to append to.
    @param value The c-string to append.

    @return true on success, false on allocation failure.
*/
static inline bool string_append_cstr(String* str, const char* value);

/**
    Appends a string to the end of another string.

    @param str The string to append to.
    @param value The string to append.

    @return true on success, false on allocation failure.
*/
static inline bool string_append_string(String* str, const String* value);

/**
    Appends a section of a c-string to the end of a string.

    @param str The string to append to.
    @param value The c-string to append.
    @param start The starting index of the section of the value to append.
    @param count The number of characters following start to append.

    @return true on success, false on allocation failure.
*/
static inline bool string_append_cstr_part(
    String* str, 
    const char* value, 
    size_t start, 
    size_t count);

/**
    Appends a section of a string to the end of another string.

    @param str The string to append to.
    @param value The string to append.
    @param start The starting index of the section of the value to append.
    @param count The number of characters following start to append.

    @return true on success, false on allocation failure.
*/
static inline bool string_append_string_part(
    String* str, 
    const String* value, 
    size_t start, 
    size_t count);

/**
    Compares a string and a c-string in the same fashion as strcmp.

    @param str The string on the left side of the operation.
    @param value The c-string on the right side of the operation.

    @return A negative value if str < value, zero if str == value, a positive value if str > value.
*/
static inline int string_compare_cstr(const String* str, const char* value);

/**
    Compares two strings in the same fashion as strcmp.

    @param str The string on the left side of the operation.
    @param value The string on the right side of the operation.

    @return A negative value if str < value, zero if str == value, a positive value if str > value.
*/
static inline int string_compare_string(const String* str, const String* value);

/**
    Determines if the contents of a String is equivalent to a c-string.

    @param str The string on the left side of the operation.
    @param value The c-string on the right side of the operation.

    @return true if the values are equivalent; false otherwise.
*/
static inline bool string_equals_cstr(const String* str, const char* value);

/**
    Determines if the contents of two strings are equivalent.

    @param str The string on the left side of the operation.
    @param value The string on the right side of the operation.

    @return true if the values are equivalent; false otherwise.
*/
static inline bool string_equals_string(const String* str, const String* value);

/**
    Determines if a string starts with the characters in a c-string.

    @param str The string to check the beginning of.
    @param value The value to check the beginning of the string for.

    @return true if the string starts with the value; false otherwise.
*/
static inline bool string_starts_with_cstr(const String* str, const char* value);

/**
    Determines if a string starts with the characters in another string.

    @param str The string to check the beginning of.
    @param value The value to check the beginning of the string for.

    @return true if the string starts with the value; false otherwise.
*/
static inline bool string_starts_with_string(const String* str, const String* value);

/**
    Determines if a string ends with the characters in a c-string.

    @param str The string to check the ending of.
    @param value The value to check the ending of the string for.

    @return true if the string ends with the value; false otherwise.
*/
static inline bool string_ends_with_cstr(const String* str, const char* value);

/**
    Determines if a string ends with the characters in another string.

    @param str The string to check the ending of.
    @param value The value to check the ending of the string for.

    @return true if the string ends with the value; false otherwise.
*/
static inline bool string_ends_with_string(const String* str, const String* value);

/**
    Replaces a section of a string with the characters in a c-string.

    @param str The string whose contents are to be replaced.
    @param pos The starting position of the contents to replace.
    @param count The number of characters following pos to replace.
    @param value The c-string value to replace the section with.

    @return true on success, false on allocation failure.
*/
static inline bool string_replace_cstr(String* str, size_t pos, size_t count, const char* value);

/**
    Replaces a section of a string with the characters in another string.

    @param str The string whose contents are to be replaced.
    @param pos The starting position of the contents to replace.
    @param count The number of characters following pos to replace.
    @param value The string value to replace the section with.

    @return true on success, false on allocation failure.
*/
static inline bool string_replace_string(String* str, size_t pos, size_t count, const String* value);

/**
    Initializes a string with the data from a slice of another string.

    @param str The string to get the slice from.
    @param pos The starting position of the slice.
    @param count The number of characters following pos to copy into the substring.
    @param out_value The string that will be initialized with the contents of the substring.
                     This value should not be initialized by the caller, or it might cause a memory leak.

    @return true on success, false on allocation failure.
*/
static inline bool string_substring(const String* str, size_t pos, size_t count, String* out_value);

/**
    Initializes a string with the data of another string.

    @param str The string to copy.
    @param out_value The string to copy the contents into.
                     This value should not be initialized by the caller, or it might cause a memory leak.

    @return true on success, false on allocation failure.
*/
static inline bool string_copy(const String* str, String* out_value);

/**
    Copies the data from a slice of a string into a c-string, overwriting any 
    previous data. Does not add a terminating character at the end.

    @param str The string to copy.
    @param cstr The c-string to copy into.
    @param pos The starting position of the string to start copying into the c-string.
    @param count The number of characters following pos to copy into the c-string.
*/
static inline void string_copy_to(const String* str, char* cstr, size_t pos, size_t count);

/**
    Resizes a string, adding the specified character to fill any new spots. 
    Removes trailing characters if the new size is smaller than the current 
    size.

    @param str The string to resize.
    @param count The new size of the string.
    @param ch The character to fill anyh new spots with.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_resize(String* str, size_t count, char ch);

/**
    Swaps the contents of two strings.

    @param left The string on the left side of the operation.
    @param right The string on the right side of the operation.
*/
static inline void string_swap(String* left, String* right);

/**
    Finds the starting index of the first occurrence of a c-string in a string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The c-string value to search for.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_find_cstr(const String* str, size_t pos, const char* value);

/**
    Finds the starting index of the first occurrence of a string in another string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The string value to search for.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_find_string(const String* str, size_t pos, const String* value);

/**
    Finds the starting index of the first occurrence of part of a c-string in a string.
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The c-string value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_find_substr_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length);

/**
    Finds the starting index of the first occurrence of part of a string in another string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The string value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_find_substr_string(const String* str, size_t pos, const String* value, size_t start, size_t length);

/**
    Finds the starting index of the last occurrence of a c-string in a string.
    
    @param str The string to search.
    @param pos The starting position in the string to start searching, starting from the back.
    @param value The c-string value to search for.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_rfind_cstr(const String* str, size_t pos, const char* value);

/**
    Finds the starting index of the last occurrence of a string in another string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching, starting from the back.
    @param value The string value to search for.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_rfind_string(const String* str, size_t pos, const String* value);

/**
    Finds the starting index of the last occurrence of part of a c-string in a string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching, starting from the back.
    @param value The c-string value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_rfind_substr_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length);

/**
    Finds the starting index of the last occurrence of part of a string in another string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching, starting from the back.
    @param value The string value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_rfind_substr_string(const String* str, size_t pos, const String* value, size_t start, size_t length);

/**
    Reverses the bytes in-place in a string.

    @param str The string to reverse.
*/
SSO_STRING_EXPORT void string_reverse_bytes(String* str);

/**
    Reverses the contents of a string in-place based on UTF-8 codepoints.

    @param str The string to reverse.
*/
SSO_STRING_EXPORT void string_u8_reverse_codepoints(String* str);

/**
    Joins an array of strings together into a single string with a
    common separator in between each of them.

    @param str The string that stores the result. If it has a value,
               the result is appended to the end. It needs to be initialized.

    @param separator  A string to separate each array value in the result.
    @param values  An array of strings to combine.
    @param value_count  The number of strings in the values array.
*/
SSO_STRING_EXPORT bool string_join(
    String* str, 
    const String* separator,
    const String* values,
    size_t value_count);

/**
    Joins an array of strings together into a single string with a
    common separator in between each of them.

    @param str The string that stores the result. If it has a value,
               the result is appended to the end. It needs to be initialized.

    @param separator A string to separate each array value in the result.
    @param values An array of string references to combine.
    @param value_count The number of strings in the values array.
*/
SSO_STRING_EXPORT bool string_join_refs(
    String* str,
    const String* separator,
    const String** values,
    size_t value_count);

/** 
    A constant that can be used to indicate that a string split function will allocate the result. 
    Should be passed as the results_count parameter.
*/
#define STRING_SPLIT_ALLOCATE -1

/**
    Splits a string into segments based on a separator.

    @param str The string to split apart.
    @param separator The string to split on.
    @param results A contiguous array of strings that will store the split segments. 
                   Should be NULL if results_count is a negative number.

    @param results_count The number of elements available in the results array. If this is
                         a negative number, the return value will be allocated by the function.

    @param results_filled A pointer that will contain the number of string segments added to the results array.
    @param skip_empty Determines if empty elements should be skipped or added to the results array.
    @param init_results Determines if the segment strings need to be initialized by this function.

    @return The value passed to results if results_count is a positive number. Otherwise, it's an array created
            by this function that will need to be manually freed. Either way, an array containing the 
            split string segments. Returns NULL on error.
*/
SSO_STRING_EXPORT String* string_split(
    const String* str,
    const String* separator,
    String* results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool init_results);

/**
    Splits a string into segments based on a separator.

    @param str The string to split apart.
    @param separator The string to split on.
    @param results A non-contiguous array of strings that will store the split segments. 
                   Should be NULL if results_count is a negative number.

    @param results_count The number of elements available in the results array. If this is
                         a negative number, the return value will be allocated by the function.

    @param results_filled A pointer that will contain the number of string segments added to the results array.
    @param skip_empty Determines if empty elements should be skipped or added to the results array.
    @param init_results Determines if the segment strings need to be allocated by this function.
                        If this is false, the results will need to be allocated/initialized beforehand.

    @return The value passed to results if results_count is a positive number. Otherwise, it's an array created
            by this function that will need to be manually freed. Either way, an array containing the 
            split string segments. Returns NULL on error.
*/
SSO_STRING_EXPORT String** string_split_refs(
    const String* str,
    const String* separator,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool allocate_results);

/**
    Formats a string using printf format specifiers.

    @param result A string that stores the result of the format operation.
                    If this is NULL, this function allocates a string for the return value.
                    Otherwise it appends the formatted data to the end.

    @param format A string that contains the text and format specifiers to be written.
    @param ... The format specifier values.

    @return result if the argument was non-null. Otherwise a newly allocated string
            that contains the format result. NULL on error.
*/
SSO_STRING_EXPORT String* string_format_string(String* result, const String* format, ...);

/**
    Formats a string using printf format specifiers.

    @param result A string that stores the result of the format operation.
                    If this is NULL, this function allocates a string for the return value.
                    Otherwise it appends the formatted data to the end.

    @param format A c-string that contains the text and format specifiers to be written.
    @param ... The format specifier values.

    @return result if the argument was non-null. Otherwise a newly allocated string
            that contains the format result. NULL on error.
*/
SSO_STRING_EXPORT String* string_format_cstr(String* result, const char* format, ...);

/**
    Formats a string using printf format specifiers.

    @param result A string that stores the result of the format operation.
                    If this is NULL, this function allocates a string for the return value.
                    Otherwise it appends the formatted data to the end.

    @param format A string that contains the text and format specifiers to be written.
    @param argp A list of the variadic arguments originally passed to a variadic function. 
                The position of the argument may be changed, so pass a copy to this method if that isn't desired.

    @return result if the argument was non-null. Otherwise a newly allocated string
            that contains the format result. NULL on error.
*/
SSO_STRING_EXPORT String* string_format_args_string(String* result, const String* format, va_list argp);

/**
    Formats a string using printf format specifiers.

    @param result A string that stores the result of the format operation.
                    If this is NULL, this function allocates a string for the return value.
                    Otherwise it appends the formatted data to the end.

    @param format A c-string that contains the text and format specifiers to be written.
    @param argp A list of the variadic arguments originally passed to a variadic function. 
                The position of the argument may be changed, so pass a copy to this method if that isn't desired.

    @return result if the argument was non-null. Otherwise a newly allocated string
            that contains the format result. NULL on error.
*/
SSO_STRING_EXPORT String* string_format_args_cstr(String* result, const char* format, va_list argp);

/**
    Creates a hash code from a string using the fnv1-a algorithm.

    @param str The string to generate a hash for.

    @return A hash code for the string.
*/
SSO_STRING_EXPORT size_t string_hash(String* str);



// Internal Functions
//
// These can technically be called by the user, but should be avoided if possible.
//
// The string_set_size functions are especially useful when interfacing with an api
// that writes to a c-string. The internal contents of the string can be overwritten
// using the api, and the size can be safely updated by the caller. WARNING: make sure
// the buffer is big enough using string_reserve when performing this type of operation.

#define SSO_STRING_SHORT_RESERVE_FAIL 0
#define SSO_STRING_SHORT_RESERVE_SUCCEED 1
#define SSO_STRING_SHORT_RESERVE_RESIZE 2

static inline size_t sso_string_next_cap(size_t current, size_t desired);
static inline bool sso_string_is_long(const String* str);
static inline size_t sso_string_short_size(const String* str);
static inline size_t sso_string_short_cap(const String* str);
static inline size_t sso_string_long_size(const String* str);
static inline size_t sso_string_long_cap(const String* str);
static inline void sso_string_long_set_cap(String* str, size_t cap);
static inline void sso_string_long_set_size(String* str, size_t size);
static inline void sso_string_short_set_size(String* str, size_t size);
SSO_STRING_EXPORT bool sso_string_long_reserve(String* str, size_t reserve);
SSO_STRING_EXPORT int sso_string_short_reserve(String* str, size_t reserve);
SSO_STRING_EXPORT bool sso_string_insert_impl(String* str, const char* value, size_t index, size_t length);
SSO_STRING_EXPORT bool sso_string_append_impl(String* str, const char* value, size_t length);
SSO_STRING_EXPORT bool sso_string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length);
SSO_STRING_EXPORT size_t sso_string_find_impl(const String* str, size_t pos, const char* value, size_t length);
SSO_STRING_EXPORT size_t sso_string_find_substr_impl(const String* str, size_t pos, const char* value, size_t length);
SSO_STRING_EXPORT size_t sso_string_rfind_impl(const String* str, size_t pos, const char* value, size_t length);





// The following functions have a size small enough to be inlined, so they are defined here in the header.

static inline size_t sso_string_next_cap(size_t current, size_t desired) {
    if(current > desired)
        return current;
    current *= 2;
    current = (size_t)max(current, desired);
    return current < STRING_MAX ? current : STRING_MAX;
}

static inline bool sso_string_is_long(const String* str) {
    return str->s.size & SSO_STRING_LONG_FLAG;
}

static inline size_t sso_string_short_size(const String* str) {
#ifdef SSO_STRING_LITTLE_ENDIAN
    return str->s.size >> 1;
#else
    return str->s.size;
#endif
}

static inline size_t sso_string_short_cap(const String* str) {
    return SSO_STRING_MIN_CAP;
}

static inline size_t sso_string_long_size(const String* str) {
    return str->l.size;
}

static inline size_t sso_string_long_cap(const String* str) {
#ifdef SSO_STRING_LITTLE_ENDIAN
    return str->l.cap >> 1;
#else
    return str->l.cap & ~((size_t)SSO_STRING_LONG_FLAG << SSO_STRING_SHIFT);
#endif
}

static inline void sso_string_long_set_cap(String* str, size_t cap) {
#ifdef SSO_STRING_LITTLE_ENDIAN
        str->l.cap = (cap << 1);
#else
        str->l.cap = cap;
#endif
        str->s.size |= SSO_STRING_LONG_FLAG;
}

static inline void sso_string_long_set_size(String* str, size_t size) {
    str->l.size = size;
}

static inline void sso_string_short_set_size(String* str, size_t size) {
#ifdef SSO_STRING_LITTLE_ENDIAN
    str->s.size = (size << 1);
#else
    str->s.size = size;
#endif
}

static inline String string_create(const char* cstr) {
    String str;
    string_init(&str, cstr);
    return str;
}

static inline String* string_create_ref(const char* cstr) {
    String* str = malloc(sizeof(String));

    if(!str || !string_init(str, cstr)) {
        free(str);
        return NULL;
    }
    return str;
}

static inline void string_free_resources(String* str) {
    if(str && sso_string_is_long(str)) {
        free(str->l.data);
    }
}

static inline void string_free(String* str) {
    string_free_resources(str);
    free(str);
}

static inline char* string_cstr(String* str) {
    return sso_string_is_long(str) ? str->l.data : str->s.data;
}

static inline const char* string_data(const String* str) {
    return sso_string_is_long(str) ? str->l.data : str->s.data;
}

static inline size_t string_size(const String* str) {
    return sso_string_is_long(str) ? sso_string_long_size(str) : sso_string_short_size(str);
}

static inline size_t string_capacity(const String* str) {
    return sso_string_is_long(str) ? sso_string_long_cap(str) : sso_string_short_cap(str);
}

static inline char string_get(const String* str, size_t index) {
    SSO_STRING_ASSERT_ARG(str);
    if(sso_string_is_long(str)) {
        SSO_STRING_ASSERT_BOUNDS(index < sso_string_long_size(str));
        return str->l.data[index];
    } else {
        SSO_STRING_ASSERT_BOUNDS(index < sso_string_short_size(str));
        return str->s.data[index];
    }
}

static inline void string_set(String* str, size_t index, char value) {
    SSO_STRING_ASSERT_ARG(str);
    if(sso_string_is_long(str)) {
        SSO_STRING_ASSERT_BOUNDS(index < sso_string_long_size(str));
        str->l.data[index] = value;
    } else {
        SSO_STRING_ASSERT_BOUNDS(index < sso_string_short_size(str));
        str->s.data[index] = value;
    }
}


static inline bool string_empty(const String* str) {
    SSO_STRING_ASSERT_ARG(str);
    return sso_string_is_long(str) ? (sso_string_long_size(str) == 0) 
                                      : (sso_string_short_size(str) == 0);
}

static inline bool string_reserve(String* str, size_t reserve) {
    SSO_STRING_ASSERT_ARG(str);
    if(sso_string_is_long(str)) {
        return sso_string_long_reserve(str, reserve);
    } else {
        return sso_string_short_reserve(str, reserve) != SSO_STRING_SHORT_RESERVE_FAIL;
    }
}

static inline void string_clear(String* str) {
    SSO_STRING_ASSERT_ARG(str);

    if(sso_string_is_long(str)) {
        str->l.data[0] = 0;
        sso_string_long_set_size(str, 0);
    } else {
        str->s.data[0] = 0;
        sso_string_short_set_size(str, 0);
    }
}

static inline bool string_insert_cstr(String* str, const char* value, size_t index) {
    return sso_string_insert_impl(str, value, index, strlen(value));
}

static inline bool string_insert_string(String* str, const String* value, size_t index) {
    return sso_string_insert_impl(str, string_data(value), index, string_size(value));
}

static inline bool string_insert_cstr_part(
    String* str, 
    const char* value, 
    size_t index,
    size_t start, 
    size_t length) 
{
    return sso_string_insert_impl(str, value + start, index, length);
}

static inline bool string_insert_string_part(
    String* str, 
    const String* value, 
    size_t index, 
    size_t start, 
    size_t length)
{
    return sso_string_insert_impl(str, string_data(value) + start, index, length);
}

static inline bool string_append_cstr(String* str, const char* value) {
    return sso_string_append_impl(str, value, strlen(value));
}

static inline bool string_append_string(String* str, const String* value) {
    return sso_string_append_impl(str, string_data(value), string_size(value));
}

static inline bool string_append_cstr_part(
    String* str, 
    const char* value, 
    size_t start, 
    size_t count)
{
    return sso_string_append_impl(str, value + start, count);
}

static inline bool string_append_string_part(
    String* str, 
    const String* value, 
    size_t start, 
    size_t count)
{
    return sso_string_append_impl(str, string_data(value) + start, count);
}

static inline int sso_string_compare_impl(const String* str, const char* value, size_t length) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(value);
    size_t size = string_size(str);
    if(size != length)
        return size < length ? -1 : 1;

    return strncmp(string_data(str), value, length);
}

static inline int string_compare_cstr(const String* str, const char* value) {
    return sso_string_compare_impl(str, value, strlen(value));
}

static inline int string_compare_string(const String* str, const String* value) {
    return sso_string_compare_impl(str, string_data(value), string_size(value));
}

static inline bool string_equals_cstr(const String* str, const char* value) {
    return string_compare_cstr(str, value) == 0;
}

static inline bool string_equals_string(const String* str, const String* value) {
    return string_compare_string(str, value) == 0;
}

static inline bool sso_string_starts_with_impl(const String* str, const char* value, size_t length) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(value);
    size_t size = string_size(str);
    if(length > size)
        return false;

    return strncmp(string_data(str), value, length) == 0;
}


static inline bool string_starts_with_cstr(const String* str, const char* value) {
    return sso_string_starts_with_impl(str, value, strlen(value));
}

static inline bool string_starts_with_string(const String* str, const String* value) {
    return sso_string_starts_with_impl(str, string_data(value), string_size(value));
}

static inline bool sso_string_ends_with_impl(const String* str, const char* value, size_t length) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(value);
    size_t size = string_size(str);
    if(length > size)
        return false;

    return strncmp(string_data(str) + (size - length), value, length) == 0;
}

static inline bool string_ends_with_cstr(const String* str, const char* value) {
    return sso_string_ends_with_impl(str, value, strlen(value));
}

static inline bool string_ends_with_string(const String* str, const String* value) {
    return sso_string_ends_with_impl(str, string_data(value), string_size(value));
}


static inline bool string_replace_cstr(String* str, size_t pos, size_t count, const char* value) {
    return sso_string_replace_impl(str, pos, count, value, strlen(value));
}


static inline bool string_replace_string(String* str, size_t pos, size_t count, const String* value) {
    return sso_string_replace_impl(str, pos, count, string_data(value), string_size(value));
}

static inline bool string_substring(const String* str, size_t pos, size_t count, String* value) {
    SSO_STRING_ASSERT_BOUNDS(pos + count <= string_size(str));
    return string_init_size(value, string_data(str) + pos, count);
}

static inline bool string_copy(const String* str, String* out_value) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(out_value);
    return string_init(out_value, string_data(str));
}

static inline void string_copy_to(const String* str, char* cstr, size_t pos, size_t count) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(cstr);
    SSO_STRING_ASSERT_BOUNDS(pos + count <= string_size(str));
    memmove(cstr, string_data(str) + pos, count);
}

static inline void string_swap(String* left, String* right) {
    SSO_STRING_ASSERT_ARG(left);
    SSO_STRING_ASSERT_ARG(right);
    String temp = *right;
    memmove(right, left, sizeof(String));
    memmove(left, &temp, sizeof(String));
}

static inline size_t string_find_cstr(const String* str, size_t pos, const char* value) {
    return sso_string_find_impl(str, pos, value, strlen(value));
}

static inline size_t string_find_string(const String* str, size_t pos, const String* value) {
    return sso_string_find_impl(str, pos, string_data(value), string_size(value));
}


static inline size_t string_find_substr_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length) {
    return sso_string_find_substr_impl(str, pos, value + start, length);
}

static inline size_t string_find_substr_string(const String* str, size_t pos, const String* value, size_t start, size_t length) {
    return sso_string_find_substr_impl(str, pos, string_data(value) + start, length);
}

static inline size_t string_rfind_cstr(const String* str, size_t pos, const char* value) {
    return sso_string_rfind_impl(str, pos, value, strlen(value));
}

static inline size_t string_rfind_string(const String* str, size_t pos, const String* value) {
    return sso_string_rfind_impl(str, pos, string_data(value), string_size(value));
}

static inline size_t string_rfind_substr_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length) {
    return sso_string_rfind_impl(str, pos, value + start, length);
}

static inline size_t string_rfind_substr_string(const String* str, size_t pos, const String* value, size_t start, size_t length) {
    return sso_string_rfind_impl(str, pos, string_data(value) + start, length);
}

static inline bool string_is_null_or_empty(const String* str) {
    return !str || string_size(str) == 0;
}

// If C11 is available, use the _Generic macro to select the correct
// string function, otherwise just default to using cstrings.

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)

#define string_insert(str, value, index) \
    _Generic((value), \
        char*: string_insert_cstr, \
        const char*: string_insert_cstr, \
        String*: string_insert_string, \
        const String*: string_insert_string) \
    ((str), (value), (index)) 

#define string_insert_part(str, value, index, start, length) \
    _Generic((value), \
        char*: string_insert_cstr_part, \
        const char*: string_insert_cstr_part, \
        String*: string_insert_string_part, \
        const String*: string_insert_string_part) \
    ((str), (value), (index), (start), (length))

#define string_append(str, value)  \
    _Generic((value), \
        char*: string_append_cstr, \
        const char*: string_append_cstr, \
        String*: string_append_string, \
        const String*: string_append_string) \
    ((str), (value))

#define string_equals(str, value) \
    _Generic((value), \
        char*: string_equals_cstr, \
        const char*: string_equals_cstr, \
        String*: string_starts_with_string, \
        const String*: string_starts_with_string) \
    ((str), (value))

#define string_compare(str, value) \
    _Generic((value), \
        char*: string_compare_cstr, \
        const char*: string_compare_cstr, \
        String*: string_compare_string, \
        const String*: string_compare_string) \
    ((str), (value))

#define string_starts_with(str, value)  \
    _Generic((value),  \
        char*: string_starts_with_cstr,  \
        const char*: string_starts_with_cstr,  \
        String*: string_starts_with_string, \
        const String*: string_starts_with_string) \
    ((str), (value))

#define string_ends_with(str, value)  \
    _Generic((value),  \
        char*: string_ends_with_cstr,  \
        const char*: string_ends_with_cstr,  \
        String*: string_ends_with_string, \
        const String*: string_ends_with_string) \
    ((str), (value))

#define string_replace(str, pos, count, value)  \
    _Generic((value),  \
        char*: string_replace_cstr,  \
        const char*: string_replace_cstr,  \
        String*: string_replace_string, \
        const String*: string_replace_string) \
    ((str), (pos), (count), (value))

#define string_find(str, pos, value)  \
    _Generic((value),  \
        char*: string_find_cstr,  \
        const char*: string_find_cstr,  \
        String*: string_find_string, \
        const String*: string_find_string) \
    ((str), (pos), (value))

#define string_find_substr(str, pos, value, start, count) \
    _Generic((value) \
        char*: string_find_substr_cstr, \
        const char*: string_find_substr_cstr, \
        String* string_find_substr_string, \
        const String*: string_find_substr_string)\
    ((str), (pos), (value), (start), (count))

#define string_rfind(str, pos, value) \
    _Generic((value),  \
        char*: string_rfind_cstr,  \
        const char*: string_rfind_cstr,  \
        String*: string_rfind_string, \
        const String*: string_rfind_string) \
    ((str), (pos), (value))

#define string_rfind_part(str, pos, value, start, count) \
    _Generic((value),  \
        char*: string_rfind_part_cstr,  \
        const char*: string_rfind_part_cstr,  \
        String*: string_rfind_part_string, \
        const String*: string_rfind_part_string) \
    ((str), (pos), (value), (start), (count))

#define string_format(str, format, ...) \
    _Generic((format),  \
        char*: string_format_cstr,  \
        const char*: string_format_cstr,  \
        String*: string_format_string, \
        const String*: string_format_string) \
    ((str), (format), __VA_ARGS__)

#define string_format_args(str, format, ...) \
    _Generic((format),  \
        char*: string_format_args_cstr,  \
        const char*: string_format_args_cstr,  \
        String*: string_format_args_string, \
        const String*: string_format_args_string) \
    ((str), (format), __VA_ARGS__)

#else

#define string_insert(str, value, index) string_insert_cstr(str, value, index)
#define string_insert_part(str, value, index, start, length) string_insert_cstr_part(str, value, index, start, length)
#define string_append(str, value) string_append_cstr(str, value)
#define string_equals(str, value) string_equals_cstr(str, value)
#define string_compare(str, value) string_compare_cstr(str, value)
#define string_starts_with(str, value) string_starts_with_cstr(str, value)
#define string_ends_with(str, value) string_ends_with_cstr(str, value)
#define string_replace(str, pos, count, value) string_replace_cstr(str, pos, count, value)
#define string_find(str, pos, value) string_find_cstr(str, pos, value)
#define string_rfind(str, pos, value) string_rfind_cstr(str, pos, value)
#define string_format(str, format, ...) string_format_cstr(str, format, __VA_ARGS__)
#define string_format_args(str, format, argp) string_format_args_cstr(str, format, argp)

#endif // C11

#endif