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
#include <time.h>

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

/**
    Assert macro used to ensure args are appropriate. 
    Typically used for NULL checks. Uses assert by default 
    so the checks are excluded from release builds.

    @see assert
*/
#define SSO_STRING_ASSERT_ARG assert
#endif

#ifndef SSO_STRING_ASSERT_BOUNDS

/**
    Assert macro used to ensure args don't go outside of string/array bounds. 
    Uses assert by default so the checks are excluded from release builds.

    @see assert
*/
#define SSO_STRING_ASSERT_BOUNDS assert
#endif

// Users can define sso_string_malloc, sso_string_realloc, and sso_string
// to use custom allocators. Mainly for use in embedded environments.
#ifndef sso_string_malloc

/**
    Custom string malloc method. Uses stdlib malloc by default.

    @see sso_string_realloc
    @see sso_string_free
    @see malloc
*/
#define sso_string_malloc malloc

/**
    Custom string realloc function. Uses stdlib realloc by default.

    @see sso_string_malloc
    @see sso_string_free
    @see realloc
*/
#define sso_string_realloc realloc

/**
    Custom string free function. Uses stdlib free by default.

    @see sso_string_malloc
    @see sso_string_free
    @see free
*/
#define sso_string_free free

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
    unsigned char size;
    char data[SSO_STRING_MIN_CAP + 1];
};

/**
    A memory efficient representation of text that can easily interop with traditional c-strings.
*/
typedef union String {
    struct sso_string_long l;
    struct sso_string_short s;
} String;

/*
    Creates an empty string l-value through a compound literal. 
    Only use with compilers that support compound literals.
*/
#define STRING_EMPTY (String){ .s={.size=0, .data={'\0'}} }

/**
    A numeric representation of a unicode character/codepoint.
*/
typedef uint32_t Char32;

/**
    Describes the result of trying to read a file into a string, 
    specifically for string_file_read_line.
 */
enum StringFileReadResult {
    /// The file read operation was successful with no errors.
    STRING_FILE_READ_RESULT_SUCCESS,

    /** 
        The file has reached the end of the file. This is a successful
        operation if the calling function returned true.
    */
    STRING_FILE_READ_RESULT_EOF,

    /**
        There was an issue pushing data to the end of a string
        caused by running out of memory.
    */
    STRING_FILE_READ_RESULT_OUT_OF_MEMORY,

    /**
        There was an issue reading from the file. Get the cause by
        calling ferror/perror/strerror.
    */
    STRING_FILE_READ_RESULT_ERROR
};

/**
    Describes the current state of a file read operation.
    
    Primarily used by string_file_read_line.
*/
struct StringFileReadState {
    /// The buffer that data from the file is read into.
    char* buffer;

    /// The size of the buffer.
    int buffer_size;

    /** 
        The index of the last new line character that 
        was encountered or -1 if there is no line break in the buffer.
    */
    int new_line_index;

    /**
        The number of characters previously read into buffer.
    */
    int max;

    /**
        The result of the last read operation. 
        
        @remark This will be set to STRING_FILE_READ_RESULT_EOF as soon as the 
                last line is read, even though string_file_read_line will still 
                return true. Reading from the file when this is not 
                STRING_FILE_READ_RESULT_SUCCESS is ok, and will just return false.
    */
    enum StringFileReadResult result;
};

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

    @remarks This function can be used to easily iterate over a UTF-8 string.
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

SSO_STRING_EXPORT void string_upper(String* str);

SSO_STRING_EXPORT void string_u8_upper(String* str);

SSO_STRING_EXPORT void string_lower(String* str);

SSO_STRING_EXPORT void string_u8_lower(String* str);

/**
    Determines if a string has no characters.

    @param str The string to check is empty.

    @return true if the string is empty; false otherwise.
*/
static inline bool string_empty(const String* str);

/**
    Determines if the string is NULL or has no characters.

    @param str The string to check is NULL or empty.

    @return true if the string is NULL or empty; false otherwise.
*/
static inline bool string_is_null_or_empty(const String* str);

/**
    Working with ASCII or UTF-8 strings, determines if the string is NULL, 
    empty, or comprised of only whitespace characters.

    @param str The string to check is NULL, empty, or comprised entirely of whitespace.

    @return true if the string is NULL, empty, or comprised only of whitespace characters; false otherwise.
*/
SSO_STRING_EXPORT bool string_u8_is_null_or_whitespace(const String* str);

/**
    Ensures that a string has a capacity large enough to hold a specified number of characters. 
    Does not include any terminating characters.

    @param str - The string to potentially enlarge.
    @param reserve - The desired minimum capacity, not including the NULL terminating character.

    @return true on success, false on allocation failure.

    @remarks Mostly for internal use and for resizing the internal buffer of a string 
            before passing it into a function that modifies a c-string. Make sure to 
            use sso_string_set_size afterwards.
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
static inline bool string_insert_part_cstr(String* str, const char* value, size_t index, size_t start, size_t count);

/**
    Inserts a section of a string into another string at the specified index.

    @param str The string to insert into.
    @param value The string to insert.
    @param index The index of the string to insert the value into.
    @param start The starting index of the value to insert.
    @param count The number of characters following start to insert.

    @return true on success, false on allocation failure.
*/
static inline bool string_insert_part_string(String* str, const String* value, size_t index, size_t start, size_t count);

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
SSO_STRING_EXPORT bool string_push(String* str, char value);

/**
    Appends a unicode character to the end of a string.

    @param str The string to append to.
    @param value The unicode character to append.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_u8_push(String* str, Char32 value);

/**
    Removes a character from the end of a string and returns
    the characters value. 

    @param str The string to get the last character of.

    @return The last character of the string if any, '\\0' otherwise.
*/
SSO_STRING_EXPORT char string_pop(String* str);

/**
    Removes a unicode character from the end of a string and 
    returns the characters value.

    @param str The string to get the last unicode character of.

    @return The last unicode character of the string if any, '\\0' otherwise.
*/
SSO_STRING_EXPORT Char32 string_u8_pop(String* str);

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
static inline bool string_append_part_cstr(
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
static inline bool string_append_part_string(
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
    Compares a subsection of a string and a c-string in the same fashion as strncmp.

    @param str The on the left size of the operation.
    @param pos The starting point of str to compare.
    @param value The c-string on the right side of the operation.
    @param start The starting point of value to compare.
    @param length The total number of characters to compare.

    @return A negative value if str < value, zero if str == value, a positive value if str > value.
*/
static inline int string_compare_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length);

/**
    Compares a subsection of two strings in the same fashion as strncmp.

    @param str The on the left size of the operation.
    @param pos The starting point of str to compare.
    @param value The string on the right side of the operation.
    @param start The starting point of value to compare.
    @param length The total number of characters to compare.

    @return A negative value if str < value, zero if str == value, a positive value if str > value.
*/
static inline int string_compare_part_string(const String* str, size_t pos, const String* value, size_t start, size_t length);

/**
    Determines if the contents of a string is equivalent to a c-string.

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
    Determines if the contents of subsections of a string and a c-string are equivalent.

    @param str The on the left size of the operation.
    @param pos The starting point of str to compare.
    @param value The c-string on the right side of the operation.
    @param start The starting point of value to compare.
    @param length The total number of characters to compare.

    @return true if the values are equivalent; false otherwise.
*/
static inline bool string_equals_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length);

/**
    Determines if the contents of subsections of two strings are equivalent.

    @param str The on the left size of the operation.
    @param pos The starting point of str to compare.
    @param value The string on the right side of the operation.
    @param start The starting point of value to compare.
    @param length The total number of characters to compare.

    @return true if the values are equivalent; false otherwise.
*/
static inline bool string_equals_part_string(const String* str, size_t pos, const String* value, size_t start, size_t length);

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
    Removes all occurences of a value from the beginning and end of a string.

    @param str The string to remove the value from.
    @param value The string value to remove.
*/
static inline void string_trim_string(String* str, const String* value);

/**
    Removes all occurences of a value from the beginning and end of a string.

    @param str The string to remove the value from.
    @param value The c-string value to remove.
*/
static inline void string_trim_cstr(String* str, const char* value);

/**
    Removes all occurences of a value from the beginning a string.

    @param str The string to remove the value from.
    @param value The string value to remove.
*/
static inline void string_trim_start_string(String* str, const String* value);

/**
    Removes all occurences of a value from the beginning a string.

    @param str The string to remove the value from.
    @param value The c-string value to remove.
*/
static inline void string_trim_start_cstr(String* str, const char* value);

/**
    Removes all occurences of a value from the end of a string.

    @param str The string to remove the value from.
    @param value The string value to remove.
*/
static inline void string_trim_end_string(String* str, const String* value);

/**
    Removes all occurences of a value from the end of a string.

    @param str The string to remove the value from.
    @param value The c-string value to remove.
*/
static inline void string_trim_end_cstr(String* str, const char* value);

/**
    Removes all occurrences of any value in an array from the beginning 
    and end of a string.

    @param str The string to remove the values from.
    @param values An array of strings to remove from str.
    @param value_count The number of items in values.
*/
static inline void string_trim_any_string(String* str, String* values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the beginning 
    and end of a string.

    @param str The string to remove the values from.
    @param values An array of string referencess to remove from str.
    @param value_count The number of items in values.
*/
static inline void string_trim_any_string_refs(String* str, String** values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the beginning 
    and end of a string.

    @param str The string to remove the values from.
    @param values An array of c-strings to remove from str.
    @param value_count The number of items in values.
*/
static inline void string_trim_any_cstr(String* str, char** values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the beginning 
    of a string.

    @param str The string to remove the values from.
    @param values An array of strings to remove from str.
    @param value_count The number of items in values.
*/
SSO_STRING_EXPORT void string_trim_any_start_string(String* str, String* values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the beginning 
    of a string.

    @param str The string to remove the values from.
    @param values An array of string references to remove from str.
    @param value_count The number of items in values.
*/
SSO_STRING_EXPORT void string_trim_any_start_string_refs(String* str, String** values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the beginning 
    of a string.

    @param str The string to remove the values from.
    @param values An array of c-strings to remove from str.
    @param value_count The number of items in values.
*/
SSO_STRING_EXPORT void string_trim_any_start_cstr(String* str, char** values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the end of a string.

    @param str The string to remove the values from.
    @param values An array of strings to remove from str.
    @param value_count The number of items in values.
*/
SSO_STRING_EXPORT void string_trim_any_end_string(String* str, String* values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the end of a string.

    @param str The string to remove the values from.
    @param values An array of string references to remove from str.
    @param value_count The number of items in values.
*/
SSO_STRING_EXPORT void string_trim_any_end_string_refs(String* str, String** values, size_t value_count);

/**
    Removes all occurrences of any value in an array from the end of a string.

    @param str The string to remove the values from.
    @param values An array of c-strings to remove from str.
    @param value_count The number of items in values.
*/
SSO_STRING_EXPORT void string_trim_any_end_cstr(String* str, char** values, size_t value_count);

/**
    Pads the beggining of a string with a character until it's at 
    least the specified size.

    @param str The string to pad.
    @param value The character to pad the string with.
    @param width The minimum size of the string after it's padded.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_pad_left(String* str, char value, size_t width);

/**
    Pads the end of a string with a character until it's at 
    least the specified size.

    @param str The string to pad.
    @param value The character to pad the string with.
    @param width The minimum size of the string after it's padded.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_pad_right(String* str, char value, size_t width);

/**
    Pads the beginning of a string with a unicode character until it's at 
    least the specified codepoints long.

    @param str The string to pad.
    @param value The unicode character to pad the string with.
    @param width The minimum number of codepoints in the string after it's padded.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_u8_pad_left(String* str, Char32 value, size_t width);

/**
    Pads the end of a string with a unicode character until it's at 
    least the specified number of codepoints long.

    @param str The string to pad.
    @param value The unicode character to pad the string with.
    @param width The minimum number of codepoints in the string after it's padded.

    @return true on success, false on allocation failure.
*/
SSO_STRING_EXPORT bool string_u8_pad_right(String* str, Char32 value, size_t width);

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
    Replaces a section of a string with the characters in another string slice.

    @param str The string whose contents are to be replaced.
    @param pos The starting position of the contents to replace.
    @param count The number of characters following pos to replace.
    @param value The c-string value to replace the section with.
    @param start The starting position of value to use as a replacement.
    @param length The number of characters following start in value to use as a replacement.

    @return true on success, false on allocation failure.
*/
static inline bool string_replace_part_cstr(String* str, size_t pos, size_t count, const char* value, size_t start, size_t length);

/**
    Replaces a section of a string with the characters in another string slice.

    @param str The string whose contents are to be replaced.
    @param pos The starting position of the contents to replace.
    @param count The number of characters following pos to replace.
    @param value The string value to replace the section with.
    @param start The starting position of value to use as a replacement.
    @param length The number of characters following start in value to use as a replacement.

    @return true on success, false on allocation failure.
*/
static inline bool string_replace_part_string(String* str, size_t pos, size_t count, const String* value, size_t start, size_t length);

/**
    Initializes a string with the data from a slice of another string slice.

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
static inline size_t string_find_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length);

/**
    Finds the starting index of the first occurrence of part of a string in another string. 
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The string value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.
*/
static inline size_t string_find_part_string(const String* str, size_t pos, const String* value, size_t start, size_t length);

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
static inline bool string_join_string(
    String* str, 
    const String* separator,
    const String* values,
    size_t value_count);

/**
    Joins an array of strings together into a single string with a 
    common separator in between each of them.

    @param str The string that stores the result. If it has a value, 
               the result is appended to the end. It needs to be initialized.

    @param separator  A c-string to separate each array value in the result.
    @param values  An array of strings to combine.
    @param value_count  The number of strings in the values array.
*/
static inline bool string_join_cstr(
    String* str, 
    const char* separator,
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
static inline bool string_join_refs(
    String* str,
    const String* separator,
    const String** values,
    size_t value_count);

/**
    Joins an array of strings together into a single string with a 
    common separator in between each of them.

    @param str The string that stores the result. If it has a value, 
               the result is appended to the end. It needs to be initialized.

    @param separator A c-string to separate each array value in the result.
    @param values An array of string references to combine.
    @param value_count The number of strings in the values array.
*/
static inline bool string_join_cstr_refs(
    String* str,
    const char* separator,
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
    
    @see STRING_SPLIT_ALLOCATE
*/
static inline String* string_split_string(
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
    @param separator The c-string to split on.
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
    
    @see STRING_SPLIT_ALLOCATE
*/
static inline String* string_split_cstr(
    const String* str,
    const char* separator,
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
    
    @see STRING_SPLIT_ALLOCATE
*/
static inline String** string_split_refs_string(
    const String* str,
    const String* separator,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool allocate_results);

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
    
    @see STRING_SPLIT_ALLOCATE
*/
static inline String** string_split_refs_cstr(
    const String* str,
    const char* separator,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool allocate_results);

/**
    Helper function to help free a library allocated result from string_split.

    @param values An array of strings returned from a string_split call when 
                  results_count was less than 0.
    @param count The value of the results_filled parameter from a string_split call.

    @see string_split
*/
static inline void string_split_free(String* values, int count);

/**
    Helper function to help free a library allocated result from string_split_refs.

    @param values An array of strings returned from a string_split_refs call when 
                  results_count was less than 0.
    @param count The value of the results_filled parameter from a string_split_refs call.

    @see string_split_refs
*/
static inline void string_split_refs_free(String** values, int count);

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
    Formats a string based on a time value using strftime format specifiers.

    @param result A string that stores the result of the format operation. 
                  If this is NULL, this function allocates a string for the return value. 
                  Otherwise it appends the formatted data to the end.

    @param format  A string that contains the text and time format specifiers to be written.
    @param timeptr The time value used to format the string.

    @return result if the argument was non-null. Otherwise a newly allocated string 
            that contains the time format result. NULL on error.
*/
static inline String* string_format_time_string(String* result, const String* format, const struct tm* timeptr);

/**
    Formats a string based on a time value using strftime format specifiers.

    @param result A string that stores the result of the format operation. 
                  If this is NULL, this function allocates a string for the return value. 
                  Otherwise it appends the formatted data to the end.

    @param format  A string that contains the text and time format specifiers to be written.
    @param timeptr The time value used to format the string.

    @return result if the argument was non-null. Otherwise a newly allocated string 
            that contains the time format result. NULL on error.
*/
SSO_STRING_EXPORT String* string_format_time_cstr(String* result, const char* format, const struct tm* timeptr);

/**
    Creates a hash code from a string using the fnv1-a algorithm.

    @param str The string to generate a hash for.

    @return A hash code for the string.
*/
static inline size_t string_hash(String* str);

/**
    Allocates and initializes a new StringFileReadState with the given buffer size.

    @param buffer_size The size of the buffer that is used to stream in content from the file.

    @return A new StringFileReadState on success, NULL on allocation error. 
*/
SSO_STRING_EXPORT struct StringFileReadState* string_file_read_state_create(size_t buffer_size);

/**
    Initializes a StringFileReadState with the given buffer.

    @param read_state The StringFileReadState to initialize.
    @param buffer The buffer used to store the content read from a file.
    @param buffer_size The size of the buffer.
*/
SSO_STRING_EXPORT void string_file_read_state_init(struct StringFileReadState* read_state, char* buffer, size_t buffer_size);

/**
    Frees a StringFileReadState and its buffer.

    @param read_state The StringFileReadState to free.

    @remark If read_state is NULL, this is a no-op.
*/
SSO_STRING_EXPORT void string_file_read_state_free(struct StringFileReadState* read_state);

/**
    Reads a single line of a file into a string.

    @param str The string that will contain the line. Its contents will be overwritten.
    @param file The file to read a line from.
    @param read_state The current state of the read operation.

    @return true if any data was read into the string, false otherwise. On false, get the reason
            through read_state->result, which can be used to check if it was just the end of the file
            or if an error was experienced.
*/
SSO_STRING_EXPORT bool string_file_read_line(String* str, FILE* file, struct StringFileReadState* read_state);

/**
    Reads the entirety of a file from its current position into a string.

    @param str The string that will contain the file contents.
    @param file The file to read the contents of.

    @return true on success, false on an error. If ferror doesn't indicate an error, 
            it was an allocation error.
*/
SSO_STRING_EXPORT bool string_file_read_all(String* str, FILE* file);

// Internal Functions
//  
// These can technically be called by the user, but should be avoided if possible.
//
// The string_set_size functions are especially useful when interfacing with an api
// that writes to a c-string. The internal contents of the string can be overwritten
// using the api, and the size can be safely updated by the caller. WARNING: make sure
// the buffer is big enough using string_reserve when performing this type of operation.

/**
    Internal return code from sso_string_short_reserve that indicates that the 
    string tried to switch from short to long but failed due to lack of memory.
*/
#define SSO_STRING_SHORT_RESERVE_FAIL 0

/**
    Internal return code from sso_string_short_reserve that indicates that the 
    operation succeeded without having to switch to a long string.
*/
#define SSO_STRING_SHORT_RESERVE_SUCCEED 1

/**
    Internal return code from sso_string_short_reserve that indicates that the 
    operation succeeded and it switched from short to long.
*/
#define SSO_STRING_SHORT_RESERVE_RESIZE 2

/**
    Sets the number of bytes used by a string, not including any NULL-terminating characters.

    @param str The string to set the size of.
    @param size The size of the string, not including the NULL-terminating character.

    @remarks This is mostly for internal use, but it can be used by others 
             to synchronize the length of a string with its internal representation 
             after some operation modified the c-string directly.
*/
static inline void sso_string_set_size(String* str, size_t size);

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
SSO_STRING_EXPORT void sso_string_trim_start_impl(String* str, const char* value, size_t length);
SSO_STRING_EXPORT void sso_string_trim_end_impl(String* str, const char* value, size_t length);
SSO_STRING_EXPORT bool sso_string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length);
SSO_STRING_EXPORT size_t sso_string_find_impl(const String* str, size_t pos, const char* value, size_t length);
SSO_STRING_EXPORT size_t sso_string_find_part_impl(const String* str, size_t pos, const char* value, size_t length);
SSO_STRING_EXPORT size_t sso_string_rfind_impl(const String* str, size_t pos, const char* value, size_t length);
SSO_STRING_EXPORT bool sso_string_join_impl(
    String* str,
    const char* separator,
    size_t separator_length,
    const String* values,
    size_t value_count);
SSO_STRING_EXPORT bool sso_string_join_refs_impl(
    String* str,
    const char* separator,
    size_t separator_length,
    const String** values,
    size_t value_count);
SSO_STRING_EXPORT String* sso_string_split_impl(
    const String* str,
    const char* separator,
    size_t separator_length,
    String* results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool init_results);
SSO_STRING_EXPORT String** sso_string_split_refs_impl(
    const String* str,
    const char* separator,
    size_t separator_length,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool allocate_results);


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

static inline void sso_string_set_size(String* str, size_t size) {
    if(sso_string_is_long(str))
        sso_string_long_set_size(str, size);
    else
        sso_string_short_set_size(str, size);
}

static inline String string_create(const char* cstr) {
    String str;
    string_init(&str, cstr);
    return str;
}

static inline String* string_create_ref(const char* cstr) {
    String* str = sso_string_malloc(sizeof(String));

    if(!str || !string_init(str, cstr)) {
        sso_string_free(str);
        return NULL;
    }
    return str;
}

static inline void string_free_resources(String* str) {
    if(str && sso_string_is_long(str)) {
        sso_string_free(str->l.data);
    }
}

static inline void string_free(String* str) {
    string_free_resources(str);
    sso_string_free(str);
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

static inline bool string_insert_part_cstr(
    String* str, 
    const char* value, 
    size_t index,
    size_t start, 
    size_t length) 
{
    return sso_string_insert_impl(str, value + start, index, length);
}

static inline bool string_insert_part_string(
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

static inline int sso_string_compare_impl(const String* str, const char* value) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(value);

    return strcmp(string_data(str), value);
}

static inline int string_compare_cstr(const String* str, const char* value) {
    return sso_string_compare_impl(str, value);
}

static inline int string_compare_string(const String* str, const String* value) {
    return sso_string_compare_impl(str, string_data(value));
}

static inline int sso_string_compare_part_impl(const String* str, size_t pos, const char* value, size_t length) {
    SSO_STRING_ASSERT_ARG(str);
    SSO_STRING_ASSERT_ARG(value);

    // (1)
    // It's valid to compare starting from the NULL terminating character.
    // The main motivation behind this is to make it valid to compare two
    // empty strings.
    // Early exit if pos is zero, since that is always valid.
    SSO_STRING_ASSERT_BOUNDS(pos == 0 || string_size(str) >= pos);

    return strncmp(string_data(str) + pos, value, length);
}

static inline int string_compare_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length) {
    // See (1) in sso_string_compare_impl
    SSO_STRING_ASSERT_BOUNDS(start == 0 || strlen(value) >= start);
    return sso_string_compare_part_impl(str, pos, value + start, length);
}

static inline int string_compare_part_string(const String* str, size_t pos, const String* value, size_t start, size_t length) {
    // See (1) in sso_string_compare_impl
    SSO_STRING_ASSERT_BOUNDS(start == 0 || string_size(value) >= start);
    return sso_string_compare_part_impl(str, pos, string_data(value), length);
}

static inline bool string_equals_cstr(const String* str, const char* value) {
    return string_compare_cstr(str, value) == 0;
}

static inline bool string_equals_string(const String* str, const String* value) {
    return string_compare_string(str, value) == 0;
}

static inline bool string_equals_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length) {
    return string_compare_part_cstr(str, pos, value, start, length) == 0;
}

static inline bool string_equals_part_string(const String* str, size_t pos, const String* value, size_t start, size_t length) {
    return string_compare_part_string(str, pos, value, start, length) == 0;
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

static inline void string_trim_string(String* str, const String* value) {
    sso_string_trim_end_impl(str, string_data(value), string_size(value));
    sso_string_trim_start_impl(str, string_data(value), string_size(value));
}

static inline void string_trim_cstr(String* str, const char* value) {
    size_t length = strlen(value);
    sso_string_trim_end_impl(str, value, length);
    sso_string_trim_start_impl(str, value, length);
}

static inline void string_trim_start_string(String* str, const String* value) {
    sso_string_trim_start_impl(str, string_data(value), string_size(value));
}

static inline void string_trim_start_cstr(String* str, const char* value) {
    sso_string_trim_start_impl(str, value, strlen(value));
}

static inline void string_trim_end_string(String* str, const String* value) {
    sso_string_trim_end_impl(str, string_data(value), string_size(value));
}

static inline void string_trim_end_cstr(String* str, const char* value) {
    sso_string_trim_end_impl(str, value, strlen(value));
}

static inline void string_trim_any_string(String* str, String* values, size_t value_count) {
    string_trim_any_end_string(str, values, value_count);
    string_trim_any_start_string(str, values, value_count);
}

static inline void string_trim_any_string_refs(String* str, String** values, size_t value_count) {
    string_trim_any_end_string_refs(str, values, value_count);
    string_trim_any_start_string_refs(str, values, value_count);
}

static inline void string_trim_any_cstr(String* str, char** values, size_t value_count) {
    string_trim_any_end_cstr(str, values, value_count);
    string_trim_any_start_cstr(str, values, value_count);
}

static inline bool string_replace_cstr(String* str, size_t pos, size_t count, const char* value) {
    return sso_string_replace_impl(str, pos, count, value, strlen(value));
}

static inline bool string_replace_string(String* str, size_t pos, size_t count, const String* value) {
    return sso_string_replace_impl(str, pos, count, string_data(value), string_size(value));
}

static inline bool string_replace_part_cstr(String* str, size_t pos, size_t count, const char* value, size_t start, size_t length) {
    SSO_STRING_ASSERT_BOUNDS(start + length <= strlen(value));
    return sso_string_replace_impl(str, pos, count, value + start, length);
}

static inline bool string_replace_part_string(String* str, size_t pos, size_t count, const String* value, size_t start, size_t length) {
    SSO_STRING_ASSERT_BOUNDS(start + length <= string_size(value));
    return sso_string_replace_impl(str, pos, count, string_data(value) + start, length);
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


static inline size_t string_find_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length) {
    return sso_string_find_part_impl(str, pos, value + start, length);
}

static inline size_t string_find_part_string(const String* str, size_t pos, const String* value, size_t start, size_t length) {
    return sso_string_find_part_impl(str, pos, string_data(value) + start, length);
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

static inline bool string_join_string(
    String* str, 
    const String* separator,
    const String* values,
    size_t value_count)
{
    return sso_string_join_impl(str, string_data(separator), string_size(separator), values, value_count);
}

static inline bool string_join_cstr(
    String* str, 
    const char* separator,
    const String* values,
    size_t value_count)
{
    return sso_string_join_impl(str, separator, strlen(separator), values, value_count);
}

static inline bool string_join_refs_string(
    String* str, 
    const String* separator,
    const String** values,
    size_t value_count)
{
    return sso_string_join_refs_impl(str, string_data(separator), string_size(separator), values, value_count);
}

static inline bool string_join_refs_cstr(
    String* str, 
    const char* separator,
    const String** values,
    size_t value_count)
{
    return sso_string_join_refs_impl(str, separator, strlen(separator), values, value_count);
}

static inline String* string_split_string(
    const String* str,
    const String* separator,
    String* results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool init_results)
{
    return sso_string_split_impl(
        str,
        string_data(separator),
        string_size(separator),
        results,
        results_count,
        results_filled,
        skip_empty,
        init_results);
}

static inline String* string_split_cstr(
    const String* str,
    const char* separator,
    String* results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool init_results)
{
    return sso_string_split_impl(
        str,
        separator,
        strlen(separator),
        results,
        results_count,
        results_filled,
        skip_empty,
        init_results);
}

static inline String** string_split_refs_string(
    const String* str,
    const String* separator,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool allocate_results)
{
    return sso_string_split_refs_impl(
        str,
        string_data(separator),
        string_size(separator),
        results,
        results_count,
        results_filled,
        skip_empty,
        allocate_results);
}

static inline String** string_split_refs_cstr(
    const String* str,
    const char* separator,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool allocate_results)
{
    return sso_string_split_refs_impl(
        str,
        separator,
        strlen(separator),
        results,
        results_count,
        results_filled,
        skip_empty,
        allocate_results);
}

static inline void string_split_free(String* results, int count) {
    for(int i = 0; i < count; i++)
        string_free_resources(results + i);
    
    sso_string_free(results);
}

static inline void string_split_free_refs(String** results, int count) {
    for(int i = 0; i < count; i++)
        string_free(results[i]);
    
    sso_string_free(results);
}

static inline String* string_format_time_string(String* result, const String* format, const struct tm* timeptr) {
    return string_format_time_cstr(result, string_data(format), timeptr);
}

static inline size_t string_hash(String* str) {
    SSO_STRING_ASSERT_ARG(str);

    const unsigned char* data = string_data(str);

#if SSO_STRING_SHIFT == 24
    
    size_t hash = 0x811c9dc5;
    while(*data != 0)
        hash = (*(data++) ^ hash) * 0x01000193;

#elif SSO_STRING_SHIFT == 56

    size_t hash = 0xcbf29ce484222325;
    while(*data != 0)
        hash = (*(data++) ^ hash) * 0x00000100000001B3;

#endif

    return hash;
}

#ifndef SSO_STRING_NO_GENERIC

// If C11 is available, use the _Generic macro to select the correct
// string function, otherwise just default to using cstrings.

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)

/**
    Inserts a value into a string at the specified index. 
    Generic version of string_insert_string and string_insert_cstr.

    @param str The string to insert into.
    @param value The value to insert.
    @param index The index of the string to insert the value into.

    @return true on success, false on allocation failure.

    @see string_insert_string
    @see string_insert_cstr
*/
#define string_insert(str, value, index) \
    _Generic((value), \
        char*: string_insert_cstr, \
        const char*: string_insert_cstr, \
        String*: string_insert_string, \
        const String*: string_insert_string) \
    ((str), (value), (index)) 

/**
    Inserts a section of a value into a string at the specified index. 
    Generic version of string_insert_part_string and string_insert_part_cstr.

    @param str The string to insert into.
    @param value The value to insert.
    @param index The index of the string to insert the value into.
    @param start The starting index of the value to insert.
    @param count The number of characters following start to insert.

    @return true on success, false on allocation failure.

    @see string_insert_part_string
    @see string_insert_part_cstr
*/
#define string_insert_part(str, value, index, start, length) \
    _Generic((value), \
        char*: string_insert_part_cstr, \
        const char*: string_insert_part_cstr, \
        String*: string_insert_part_string, \
        const String*: string_insert_part_string) \
    ((str), (value), (index), (start), (length))

/**
    Appends a value to the end of a string. 
    Generic version of string_append_string and string_append_cstr.

    @param str The string to append to.
    @param value The value to append.

    @return true on success, false on allocation failure.

    @see string_append_string
    @see string_append_cstr
*/
#define string_append(str, value) \
    _Generic((value), \
        char*: string_append_cstr, \
        const char*: string_append_cstr, \
        String*: string_append_string, \
        const String*: string_append_string) \
    ((str), (value))

/**
    Appends a section of a value to the end of a string. 
    Generic version of string_append_part_cstr and string_append_part_string

    @param str The string to append to.
    @param value The value to append.
    @param start The starting index of the section of the value to append.
    @param count The number of characters following start to append.

    @return true on success, false on allocation failure.

    @see string_append_string
    @see string_append_cstr
*/
#define string_append_part(str, value, start, count) \
    _Generic((value), \
        char*: string_append_part_cstr, \
        const char*: string_append_part_cstr, \
        String*: string_append_part_string, \
        const String*: string_append_part_string) \
    ((str), (value), (start), (count))

/**
    Determines if the contents of a String is equivalent to a value. 
    Generic version of string_equals_string and string_equals_cstr.

    @param str The string on the left side of the operation.
    @param value The value on the right side of the operation.

    @return true if the values are equivalent; false otherwise.

    @see string_equals_string
    @see string_equals_cstr
*/
#define string_equals(str, value) \
    _Generic((value), \
        char*: string_equals_cstr, \
        const char*: string_equals_cstr, \
        String*: string_starts_with_string, \
        const String*: string_starts_with_string) \
    ((str), (value))

/**
    Compares a string and a value in the same fashion as strcmp. 
    Generic version of string_compare_string and string_compare_cstr.

    @param str The string on the left side of the operation.
    @param value The value on the right side of the operation.

    @return A negative value if str < value, zero if str == value, a positive value if str > value.

    @see string_compare_string
    @see string_compare_cstr
*/
#define string_compare(str, value) \
    _Generic((value), \
        char*: string_compare_cstr, \
        const char*: string_compare_cstr, \
        String*: string_compare_string, \
        const String*: string_compare_string) \
    ((str), (value))

/**
    Determines if a string starts with the characters in a value. 
    Generic version of string_starts_with_string and string_starts_with_cstr.

    @param str The string to check the beginning of.
    @param value The value to check the beginning of the string for.

    @return true if the string starts with the value; false otherwise.

    @see string_starts_with_string
    @see string_starts_with_cstr
*/
#define string_starts_with(str, value) \
    _Generic((value), \
        char*: string_starts_with_cstr, \
        const char*: string_starts_with_cstr, \
        String*: string_starts_with_string, \
        const String*: string_starts_with_string) \
    ((str), (value))

/**
    Determines if a string ends with the characters in a value. 
    Generic version of string_ends_with_string and string_ends_with_cstr.

    @param str The string to check the ending of.
    @param value The value to check the ending of the string for.

    @return true if the string ends with the value; false otherwise.

    @see string_ends_with_string
    @see string_ends_with_cstr
*/
#define string_ends_with(str, value) \
    _Generic((value), \
        char*: string_ends_with_cstr, \
        const char*: string_ends_with_cstr, \
        String*: string_ends_with_string, \
        const String*: string_ends_with_string) \
    ((str), (value))

/**
    Removes all occurences of a value from the beginning and end of a string. 
    Generic version of string_trim_string and string_trim_cstr.

    @param str The string to remove the value from.
    @param value The string value to remove.

    @see string_trim_string
    @see string_trim_cstr
*/
#define string_trim(str, value) \
    _Generic((value), \
        char*: string_trim_cstr, \
        const char*: string_trim_cstr, \
        String*: string_trim_string, \
        const String*: string_trim_string) \
    ((str), (value))

/**
    Removes all occurences of a value from the beginning a string. 
    Generic version of string_trim_start_string and string_trim_start_cstr.

    @param str The string to remove the value from.
    @param value The string value to remove.

    @see string_trim_start_string
    @see string_trim_start_cstr
*/
#define string_trim_start(str, value) \
    _Generic((value), \
        char*: string_trim_start_cstr, \
        const char*: string_trim_start_cstr, \
        String*: string_trim_start_string, \
        const String*: string_trim_start_string) \
    ((str), (value))

/**
    Removes all occurences of a value from the end of a string. 
    Generic version of string_trim_end_string and string_trim_end_cstr.

    @param str The string to remove the value from.
    @param value The string value to remove.

    @see string_trim_end_string
    @see string_trim_end_cstr
*/
#define string_trim_end(str, value) \
    _Generic((value), \
        char*: string_trim_end_cstr, \
        const char*: string_trim_end_cstr, \
        String*: string_trim_end_string, \
        const String*: string_trim_end_string) \
    ((str), (value))

/**
    Removes all occurrences of any value in an array from the beginning 
    and end of a string. 
    Generic version of string_trim_any_string, string_trim_any_string_refs, 
    and string_trim_any_cstr.

    @param str The string to remove the values from.
    @param values An array of values to remove from str.
    @param value_count The number of items in values.

    @see string_trim_any_string
    @see string_trim_any_string_refs
    @see string_trim_any_cstr
*/
#define string_trim_any(str, values, value_count) \
    _Generic((values), \
        String*: string_trim_any_string, \
        String**: string_trim_any_string_refs, \
        char**: string_trim_any_cstr) \
    ((str), (values), (values_count))

/**
    Removes all occurrences of any value in an array from the beginning 
    of a string. 
    Generic version of string_trim_any_start_string, 
    string_trim_any_start_string_refs, and string_trim_any_start_cstr.

    @param str The string to remove the values from.
    @param values An array of values to remove from str.
    @param value_count The number of items in values.

    @see string_trim_any_start_string
    @see string_trim_any_start_string_refs
    @see string_trim_any_start_cstr
*/
#define string_trim_any_start(str, values, value_count) \
    _Generic((values), \
        String*: string_trim_any_start_string, \
        String**: string_trim_any_start_string_refs, \
        char**: string_trim_any_start_cstr) \
    ((str), (values), (values_count))

/**
    Removes all occurrences of any value in an array from the end of a string. 
    Generic version of string_trim_any_end_string, 
    string_trim_any_end_string_refs, and string_trim_any_end_cstr.

    @param str The string to remove the values from.
    @param values An array of strings to remove from str.
    @param value_count The number of items in values.

    @see string_trim_any_end_string
    @see string_trim_any_end_string_refs
    @see string_trim_any_end_cstr
*/
#define string_trim_any_end(str, values, value_count) \
    _Generic((values), \
        String*: string_trim_any_end_string, \
        String**: string_trim_any_end_string_refs, \
        char**: string_trim_any_end_cstr) \
    ((str), (values), (values_count))

/**
    Replaces a section of a string with the characters in a value. 
    Generic version of string_replace_string and string_replace_cstr.

    @param str The string whose contents are to be replaced.
    @param pos The starting position of the contents to replace.
    @param count The number of characters following pos to replace.
    @param value The value to replace the section with.

    @return true on success, false on allocation failure.

    @see string_replace_string
    @see string_replace_cstr
*/
#define string_replace(str, pos, count, value) \
    _Generic((value), \
        char*: string_replace_cstr, \
        const char*: string_replace_cstr, \
        String*: string_replace_string, \
        const String*: string_replace_string) \
    ((str), (pos), (count), (value))

/**
    Replaces a section of a string with the characters in another value slice. 
    Generic version of string_replace_part_string and string_replace_part_cstr.

    @param str The string whose contents are to be replaced.
    @param pos The starting position of the contents to replace.
    @param count The number of characters following pos to replace.
    @param value The value to replace the section with.
    @param start The starting position of value to use as a replacement.
    @param length The number of characters following start in value to use as a replacement.

    @return true on success, false on allocation failure.

    @see string_replace_part_string
    @see string_replace_part_cstr
*/
#define string_replace_part(str, pos, count, value, start, length) \
    _Generic((value), \
        char*: string_replace_part_cstr, \
        const char*: string_replace_part_cstr, \
        String*: string_replace_part_string, \
        const String*: string_replace_part_string) \
    ((str), (pos), (count), (value), (start), (length))

/**
    Finds the starting index of the first occurrence of a value in a string. 
    Generic version of string_find_string and string_find_cstr.
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The value to search for.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.

    @see string_find_string
    @see string_find_cstr
*/
#define string_find(str, pos, value) \
    _Generic((value), \
        char*: string_find_cstr, \
        const char*: string_find_cstr, \
        String*: string_find_string, \
        const String*: string_find_string) \
    ((str), (pos), (value))

/**
    Finds the starting index of the first occurrence of part of a value in a string. 
    Generic version of string_find_part_string and string_find_part_cstr.
    
    @param str The string to search.
    @param pos The starting position in the string to start searching.
    @param value The value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.

    @see string_find_part_string
    @see string_find_part_cstr
*/
#define string_find_part(str, pos, value, start, count) \
    _Generic((value) \
        char*: string_find_part_cstr, \
        const char*: string_find_part_cstr, \
        String* string_find_part_string, \
        const String*: string_find_part_string)\
    ((str), (pos), (value), (start), (count))

/**
    Finds the starting index of the last occurrence of a value in a string. 
    Generic version of string_rfind_string and string_rfind_cstr.
    
    @param str The string to search.
    @param pos The starting position in the string to start searching, starting from the back.
    @param value The value to search for.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.

    @see string_rfind_string
    @see string_rfind_cstr
*/
#define string_rfind(str, pos, value) \
    _Generic((value), \
        char*: string_rfind_cstr, \
        const char*: string_rfind_cstr, \
        String*: string_rfind_string, \
        const String*: string_rfind_string) \
    ((str), (pos), (value))

/**
    Finds the starting index of the last occurrence of part of a value in a string. 
    Generic version of string_rfind_part_string and string_rfind_part_cstr.
    
    @param str The string to search.
    @param pos The starting position in the string to start searching, starting from the back.
    @param value The value to search for.
    @param start The beginning index of value to search for in the string.
    @param length The number of characters following start in value to search for in the string.

    @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.

    @see string_rfind_part_string
    @see string_rfind_part_cstr
*/
#define string_rfind_part(str, pos, value, start, count) \
    _Generic((value), \
        char*: string_rfind_part_cstr, \
        const char*: string_rfind_part_cstr, \
        String*: string_rfind_part_string, \
        const String*: string_rfind_part_string) \
    ((str), (pos), (value), (start), (count))

/**
    Joins an array of strings together into a single string with a 
    common separator in between each of them. 
    Generic version of string_join_string and string_join_cstr.

    @param str The string that stores the result. If it has a value, 
               the result is appended to the end. It needs to be initialized.

    @param separator  A value to separate each array value in the result.
    @param values  An array of strings to combine.
    @param value_count  The number of strings in the values array.

    @see string_join_string
    @see string_join_cstr
*/
#define string_join(str, separator, values, value_count) \
    _Generic((separator), \
        char*: string_join_cstr, \
        const char*: string_join_cstr, \
        String*: string_join_string, \
        const String*: string_join_string) \
    ((str), (separator), (values), (value_count))

/**
    Joins an array of strings together into a single string with a 
    common separator in between each of them. 
    Generic version of string_join_refs_string and string_join_refs_cstr.

    @param str The string that stores the result. If it has a value, 
               the result is appended to the end. It needs to be initialized.

    @param separator A value to separate each array value in the result.
    @param values An array of string references to combine.
    @param value_count The number of strings in the values array.

    @see string_join_refs_string
    @see string_join_refs_cstr
*/
#define string_join_refs(str, separator, values, value_count) \
    _Generic((separator), \
        char*: string_join_refs_cstr, \
        const char*: string_join_refs_cstr, \
        String*: string_join_refs_string, \
        const String*: string_join_refs_string) \
    ((str), (separator), (values), (value_count))

/**
    Splits a string into segments based on a separator. 
    Generic version of string_split_string and string_split_cstr.

    @param str The string to split apart.
    @param separator The value to split on.
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

    @see string_split_string
    @see string_split_cstr
*/
#define string_split(str, separator, results, results_count, results_filled, skip_empty, init_results) \
    _Generic((separator), \
        char*: string_split_cstr, \
        const char*: string_split_cstr, \
        String*: string_split_string, \
        const String*: string_split_string) \
    ((str), (separator), (results), (results_count), (results_filled), (skip_empty), (init_results))

/**
    Splits a string into segments based on a separator. 
    Generic version of string_split_refs_string and string_split_refs_cstr.

    @param str The string to split apart.
    @param separator The value to split on.
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

    @see string_split_refs_string
    @see string_split_refs_cstr
*/
#define string_split_refs(str, separator, results, results_count, results_filled, skip_empty, allocate_results) \
    _Generic((separator), \
        char*: string_split_refs_cstr, \
        const char*: string_split_refs_cstr, \
        String*: string_split_refs_string, \
        const String*: string_split_refs_string) \
    ((str), (separator), (results), (results_count), (results_filled), (skip_empty), (allocate_results))

/**
    Formats a string using printf format specifiers. 
    Generic version of string_format_string and string_format_cstr.

    @param result A string that stores the result of the format operation. 
                  If this is NULL, this function allocates a string for the return value. 
                  Otherwise it appends the formatted data to the end.

    @param format A value that contains the text and format specifiers to be written.
    @param ... The format specifier values.

    @return result if the argument was non-null. Otherwise a newly allocated string 
            that contains the format result. NULL on error.

    @see string_format_string
    @see string_format_cstr
*/
#define string_format(str, format, ...) \
    _Generic((format), \
        char*: string_format_cstr, \
        const char*: string_format_cstr, \
        String*: string_format_string, \
        const String*: string_format_string) \
    ((str), (format), __VA_ARGS__)

/**
    Formats a string based on a time value using strftime format specifiers. 
    Generic version of string_format_time_string and string_format_time_cstr.

    @param result A string that stores the result of the format operation. 
                  If this is NULL, this function allocates a string for the return value. 
                  Otherwise it appends the formatted data to the end.

    @param format  A value that contains the text and time format specifiers to be written. 
    @param timeptr The time value used to format the string.

    @return result if the argument was non-null. Otherwise a newly allocated string 
            that contains the time format result. NULL on error.


    @see string_format_time_string
    @see string_format_time_cstr
*/
#define string_format_time(str, format, timeptr) \
    _Generic((format), \
        char*: string_format_time_cstr, \
        const char*: string_format_time_cstr, \
        String*: string_format_time_string, \
        const String*: string_format_time_string) \
    ((str), (format), (timeptr))

/**
    Formats a string using printf format specifiers. 
    Generic version of string_format_args_string and string_format_args_cstr.

    @param result A string that stores the result of the format operation. 
                  If this is NULL, this function allocates a string for the return value. 
                  Otherwise it appends the formatted data to the end.

    @param format A value that contains the text and format specifiers to be written.
    @param argp A list of the variadic arguments originally passed to a variadic function. 
                The position of the argument may be changed, so pass a copy to this method if that isn't desired.

    @return result if the argument was non-null. Otherwise a newly allocated string 
            that contains the format result. NULL on error.

    @see string_format_args_string
    @see string_format_args_cstr
*/
#define string_format_args(str, format, ...) \
    _Generic((format), \
        char*: string_format_args_cstr, \
        const char*: string_format_args_cstr, \
        String*: string_format_args_string, \
        const String*: string_format_args_string) \
    ((str), (format), __VA_ARGS__)

#else

#define string_insert(str, value, index) string_insert_cstr((str), (value), (index))
#define string_insert_part(str, value, index, start, length) \
    string_insert_part_cstr((str), (value), (index), (start), (length))
#define string_append(str, value) string_append_cstr((str), (value))
#define string_append_part(str, value, start, count) string_append_part_cstr(str, value, start, count)
#define string_equals(str, value) string_equals_cstr((str), (value))
#define string_compare(str, value) string_compare_cstr((str), (value))
#define string_starts_with(str, value) string_starts_with_cstr((str), (value))
#define string_ends_with(str, value) string_ends_with_cstr((str), (value))
#define string_trim(str, value) string_trim_cstr((str), (value))
#define string_trim_start(str, value) string_trim_start_cstr((str), (value))
#define string_trim_end(str, value) string_trim_end_cstr((str), (value))
#define string_trim_any(str, values, value_count) string_trim_any_cstr((str), (values), (value_count))
#define string_trim_any_start(str, values, value_count) string_trim_any_start_cstr((str), (values), (value_count))
#define string_trim_any_end(str, values, value_count) string_trim_any_end_cstr((str), (values), (value_count))
#define string_replace(str, pos, count, value) string_replace_cstr((str), (pos), (count), (value))
#define string_replace_part(str, pos, count, value, start, length) \
    string_replace_part_cstr((str), (pos), (count), (value), (start), (length))
#define string_find(str, pos, value) string_find_cstr((str), (pos), (value))
#define string_find_part(str, pos, value, start, count) \
    string_find_part_cstr((str), (pos), (value), (start), (count))
#define string_rfind(str, pos, value) string_rfind_cstr((str), (pos), (value))
#define string_rfind_part(str, pos, value, start, count) \
    string_rfind_part_cstr((str), (pos), (value), (start), (count))
#define string_join(str, separator, values, value_count) \
    string_join_cstr((str), (separator), (values), (value_count))
#define string_join_refs(str, separator, values, value_count) \
    string_join_refs_cstr((str), (separator), (values), (value_count))
#define string_split(str, separator, results, results_count, results_filled, skip_empty, init_results) \
    string_split_cstr((str), (separator), (results), (results_count), (results_filled), (skip_empty), (init_results))
#define string_split_refs(str, separator, results, results_count, results_filled, skip_empty, allocate_results) \
    string_split_refs_cstr((str), (separator), (results), (results_count), (results_filled), (skip_empty), (allocate_results))
#define string_format(str, format, ...) string_format_cstr((str), (format), __VA_ARGS__)
#define string_format_args(str, format, argp) string_format_args_cstr((str), (format), (argp))
#define string_format_time(str, format, timeptr) string_format_time_cstr((str), (format), (timeptr))

#endif // C11

#endif // SSO_STRING_NO_GENERIC

#endif