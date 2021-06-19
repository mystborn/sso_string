#include <sso_string.h>

#include <stdarg.h>

#ifdef _MSC_VER

#define SSO_THREAD_LOCAL __declspec(thread)

#elif defined(__GNUC__) || defined(__clang__)

#define SSO_THREAD_LOCAL __thread

#elif __STDC_VERSION__ >= 201112L
#ifndef __STDC_NO_THREADS__
#ifdef thread_local

#include <threads.h>

#define SSO_THREAD_LOCAL thread_local

#endif
#endif
#endif

#define U8_SINGLE 0x7F
#define U8_DOUBLE 0xE0
#define U8_TRIPLE 0xF0
#define U8_QUAD   0xF8 

static inline void ___sso_string_set_size(String* str, size_t size) {
    if(___sso_string_is_long(str))
        ___sso_string_long_set_size(str, size);
    else
        ___sso_string_short_set_size(str, size);
}

SSO_STRING_EXPORT bool string_init(String* str, const char* cstr) {
    assert(str);

    if(cstr == NULL)
        cstr = "";

    size_t len = strlen(cstr);
    if (len <= ___sso_string_min_cap) {
        memcpy(str->s.data, cstr, len);
        str->s.data[len] = 0;

        ___sso_string_short_set_size(str, len);
    } else {
        size_t cap = ___sso_string_next_cap(0, len);
        str->l.data = malloc(cap + 1);
        if(!str->l.data)
            return false;
        memcpy(str->l.data, cstr, len);
        str->l.data[len] = 0;
        str->l.size = len;
        ___sso_string_long_set_cap(str, cap);
    }

    return true;
}

SSO_STRING_EXPORT bool string_init_size(String* str, const char* cstr, size_t len) {
    if(cstr == NULL)
        cstr = "";

    assert(str);
    assert(len <= strlen(cstr));

    if (len <= ___sso_string_min_cap) {
        memcpy(str->s.data, cstr, len);
        str->s.data[len] = 0;

        ___sso_string_short_set_size(str, len);
    } else {
        size_t cap = ___sso_string_next_cap(0, len);
        str->l.data = malloc(cap + 1);
        if(!str->l.data)
            return false;
        memcpy(str->l.data, cstr, len);
        str->l.data[len] = 0;
        str->l.size = len;
        ___sso_string_long_set_cap(str, cap);
    }

    return true;
}

SSO_STRING_EXPORT size_t string_u8_codepoints(const String* str) {
    assert(str);

    const unsigned char* data = (unsigned char*)string_data(str);
    size_t count = 0;
    while(*data) {
        count++;

        if(*data  <= U8_SINGLE)
            data += 1;
        else if(*data < U8_DOUBLE)
            data += 2;
        else if(*data < U8_TRIPLE)
            data += 3;
        else
            data += 4;
    }

    return count;
}

SSO_STRING_EXPORT Char32 string_u8_get(const String* str, size_t index) {
    assert(str);

    size_t size = string_size(str);
    if(index >= size)
        return '\0';

    const unsigned char* data = (unsigned char*)string_data(str) + index;

    // TODO: Test for control characters (0x80 - 0xBF)

    if(*data <= U8_SINGLE) {
        return *data;
    } else if(*data < U8_DOUBLE) {
        return (((*data) & ~U8_DOUBLE) << 6) | ((*(data + 1)) & 0x3f);
    } else if(*data < U8_TRIPLE) {
        return (((*data) & ~U8_TRIPLE) << 12) | (((*(data + 1)) & 0x3f) << 6) | ((*(data + 2)) & 0x3f);
    } else {
        return (((*data) & ~U8_QUAD) << 18) | (((*(data + 1)) & 0x3f) << 12) | (((*(data + 2)) & 0x3f) << 6) | ((*(data + 3)) & 0x3f);
    }
}

SSO_STRING_EXPORT Char32 string_u8_get_with_size(const String* str, size_t index, int* out_size) {
    assert(str);

    if(!out_size)
        return string_u8_get(str, index);

    size_t size = string_size(str);
    if(index >= size) {
        *out_size = 0;
        return '\0';
    }

    const unsigned char* data = (unsigned char*)string_data(str) + index;

    if(*data  <= U8_SINGLE) {
        *out_size = 1;
        return *data;
    } else if(*data < U8_DOUBLE) {
        *out_size = 2;
        return (((*data) & ~U8_DOUBLE) << 6) | ((*(data + 1)) & 0x3f);
    } else if(*data < U8_TRIPLE) {
        *out_size = 3;
        return (((*data) & ~U8_TRIPLE) << 12) | (((*(data + 1)) & 0x3f) << 6) | ((*(data + 2)) & 0x3f);
    } else {
        *out_size = 4;
        return (((*data) & ~U8_QUAD) << 18) | (((*(data + 1)) & 0x3f) << 12) | (((*(data + 2)) & 0x3f) << 6) | ((*(data + 3)) & 0x3f);
    }
}

SSO_STRING_EXPORT int string_u8_codepoint_size(const String* str, size_t index) {
    assert(str);

    size_t size = string_size(str);
    if(index >= size)
        return 0;

    const char* data = string_data(str) + index;

    if((unsigned char)*data  <= U8_SINGLE)
        return 1;
    else if((unsigned char)*data < U8_DOUBLE)
        return 2;
    else if((unsigned char)*data < U8_TRIPLE)
        return 3;
    else
        return 4;
}

static bool ___sso_string_u8_assure_codepoint_space(
    String* str, 
    size_t index, 
    int old_codepoint, 
    int new_codepoint) 
{
    if(new_codepoint == old_codepoint)
        return true;

    int offset = new_codepoint - old_codepoint;
    size_t size = string_size(str);

    if(offset > 0) {
        if(!string_reserve(str, size + offset))
            return false;
    }

    char* data = string_cstr(str);
    char* ptr = data;

    memmove(data + index + new_codepoint, data + index + old_codepoint, size - index - old_codepoint);
    ptr = data;
    ___sso_string_set_size(str, size + offset);
    data[size + offset] = '\0';

    return true;
}

SSO_STRING_EXPORT bool string_u8_set(String* str, size_t index, Char32 value) {
    assert(str);

    size_t size = string_size(str);

    assert(index <= string_size(str));

    if(index == size)
        return string_u8_push_back(str, value);

    int old_codepoint = string_u8_codepoint_size(str, index);
    char* data;

    if(value < 0x80) {
        if(!___sso_string_u8_assure_codepoint_space(str, index, old_codepoint, 1))
            return false;

        data = string_cstr(str);
        data[index] = (char)value;

    } else if(value < 0x800) {
        if(!___sso_string_u8_assure_codepoint_space(str, index, old_codepoint, 2))
            return false;

        data = string_cstr(str);
        data[index]     = 0xC0 | ((value >> 6) & 0x1F);
        data[index + 1] = 0x80 | (value & 0x3f);

    } else if(value < 0x10000) {
        if(!___sso_string_u8_assure_codepoint_space(str, index, old_codepoint, 3))
            return false;

        data = string_cstr(str);

        data[index]     = 0xE0 | ((value >> 12) & 0xF);
        data[index + 1] = 0x80 | ((value >> 6)  & 0x3F);
        data[index + 2] = 0x80 | (value & 0x3F);
    } else {
        assert(value <= 0x10FFFF);
        if(!___sso_string_u8_assure_codepoint_space(str, index, old_codepoint, 4))
            return false;

        data = string_cstr(str);

        data[index]     = 0xF0 | ((value >> 18) & 0x7);
        data[index + 1] = 0x80 | ((value >> 12) & 0x3F);
        data[index + 2] = 0x80 | ((value >> 6)  & 0x3F);
        data[index + 3] = 0x80 | (value & 0x3F);
    }

    return true;
}

SSO_STRING_EXPORT void string_shrink_to_fit(String* str) {
    assert(str);

    if(!___sso_string_is_long(str))
        return;

    size_t s = ___sso_string_long_size(str);

    if(s == ___sso_string_long_cap(str))
        return;

    if(s <= ___sso_string_min_cap) {
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

SSO_STRING_EXPORT void string_clear(String* str) {
    assert(str);

    if(___sso_string_is_long(str)) {
        str->l.data[0] = 0;
        ___sso_string_long_set_size(str, 0);
    } else {
        str->s.data[0] = 0;
        ___sso_string_short_set_size(str, 0);
    }
}

SSO_STRING_EXPORT bool ___sso_string_long_reserve(String* str, size_t reserve) {
    assert(str);

    size_t current = ___sso_string_long_cap(str);
    if(reserve <= current)
        return true;

    reserve = ___sso_string_next_cap(current, reserve);
    void* buffer = realloc(str->l.data, reserve + 1);
    if(!buffer)
        return false;

    str->l.data = buffer;
    str->l.data[reserve] = 0;

    ___sso_string_long_set_cap(str, reserve);
    return true;
}

SSO_STRING_EXPORT int ___sso_string_short_reserve(String* str, size_t reserve) {
    assert(str);

    if(reserve <= ___sso_string_min_cap)
        return ___SSO_STRING_SHORT_RESERVE_SUCCEED;

    reserve = ___sso_string_next_cap(___sso_string_min_cap, reserve);
    char* data = malloc(sizeof(char) * (reserve + 1));
    if(!data)
        return ___SSO_STRING_SHORT_RESERVE_FAIL;

    memmove(data, str->s.data, ___sso_string_short_size(str));
    data[reserve] = 0;

    str->l.size = ___sso_string_short_size(str);
    str->l.data = data;

    ___sso_string_long_set_cap(str, reserve);

    return ___SSO_STRING_SHORT_RESERVE_RESIZE;
}

SSO_STRING_EXPORT bool ___sso_string_insert_impl(String* str, const char* value, size_t index, size_t length) {
    assert(str);
    assert(value);

    size_t current_size = string_size(str);

    assert(current_size + length < STRING_MAX);

    if(!string_reserve(str, current_size + length))
        return false;
    char* data = string_cstr(str);
    data[current_size + length] = 0;
    if(index != current_size)
        memmove(data + index + length, data + index, current_size - index);
    memmove(data + index, value, length);
    ___sso_string_set_size(str, current_size + length);
    return true;
}

SSO_STRING_EXPORT bool ___sso_string_append_impl(String* str, const char* value, size_t length) {
    assert(str);
    assert(value);

    size_t size = string_size(str);
    if(!string_reserve(str, size + length))
        return false;
    char* data = string_cstr(str);
    memmove(data + size, value, length);
    data[size + length] = 0;
    ___sso_string_set_size(str, size + length);
    return true;
}

SSO_STRING_EXPORT bool ___sso_string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length) {
    if(length == 0)
        string_erase(str, pos, count);

    assert(str);
    assert(value);

    size_t size = string_size(str);

    assert(pos + count <= size);

    char* data;
    if(count == length) {
        data = (char*)string_cstr(str);
        memmove(data + pos, value, length);
    } else if(count > length) {
        data = (char*)string_cstr(str);
        size_t offset = pos + count;
        memmove(data + pos + length, data + offset, size - offset);
        memmove(data + pos, value, length);
        size_t end = pos + length + (size - offset);
        data[end] = 0;
        if(___sso_string_is_long(str))
            ___sso_string_long_set_size(str, end);
        else
            ___sso_string_short_set_size(str, end);
    } else {
        size_t offset = length - count;

        if(!string_reserve(str, size + offset))
            return false;

        size_t cap = string_capacity(str);
        data = (char*)string_cstr(str);
        memmove(data + pos + length, data + pos + count, size - (pos + count));
        memmove(data + pos, value, length);
        data[size+offset] = 0;
        if(___sso_string_is_long(str))
            ___sso_string_long_set_size(str, size + offset);
        else
            ___sso_string_short_set_size(str, size + offset);
    }

    return true;
}

SSO_STRING_EXPORT bool string_u8_is_null_or_whitespace(const String* str) {
    static int whitespace_single[] = { 
        9, 10, 11, 12, 13, 32, 133, 160
    };

    static int whitespace_double[] = {
        133, 160
    };

    static int whitespace_triple[] = {
        5760,
        6158,
        8192,
        8193,
        8194,
        8195,
        8196,
        8197,
        8198,
        8199,
        8200,
        8201,
        8202,
        8203,
        8204,
        8205,
        8232,
        8233,
        8287,
        8288,
        12288,
        65279
    };

    if(string_is_null_or_empty(str))
        return true;

    size_t size = string_size(str);
    const char* data = string_data(str);
    size_t i = 0;

    while(i < size) {
        int cp;
        Char32 next = string_u8_get_with_size(str, i, &cp);
        bool is_whitespace = false;

        switch(cp) {
            case 1:
                for(int i = 0; i < sizeof(whitespace_single) / sizeof(int); i++) {
                    if(whitespace_single[i] == next) {
                        is_whitespace = true;
                        break;
                    }
                }
                break;
            case 2:
                for(int i = 0; i < sizeof(whitespace_double) / sizeof(int); i++) {
                    if(whitespace_double[i] == next) {
                        is_whitespace = true;
                        break;
                    }
                }
                break;
            case 3:
                for(int i = 0; i < sizeof(whitespace_triple) / sizeof(int); i++) {
                    if(whitespace_triple[i] == next) {
                        is_whitespace = true;
                        break;
                    }
                }
                break;
            default:
                break;
        }

        if(!is_whitespace)
            return false;

        i += cp;
    }

    return true;
}

// Todo: Attempt to use intrinsic bswap

static inline void string_reverse_bytes_impl(char* start, char* end) {
    while (start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
}

SSO_STRING_EXPORT void string_reverse_bytes(String* str) {
    assert(str);

    size_t size = string_size(str);
    char* start = string_cstr(str);
    char* end = start + size - 1;

    string_reverse_bytes_impl(start, end);
}

SSO_STRING_EXPORT void string_u8_reverse_codepoints(String* str) {
    assert(str);

    size_t size = string_size(str);
    char* data;
    char* start = data = string_cstr(str);
    char* end = start + size - 1;
    char tmp;

    // Reverse string bytewise
    string_reverse_bytes_impl(start, end);

    start = NULL;

    // Todo: Speed this up
    // Fix up utf8 characters.
    for (size_t i = 0; i < size; i++) {
        switch((*(data + i)) & 0xC0) {
            case 0xC0:
                string_reverse_bytes_impl(start, data + i);
                start = NULL;
                break;
            case 0x80:
                if(!start)
                    start = data + i;
                break;
        }
    }
}

SSO_STRING_EXPORT bool string_join(
    String* str, 
    const String* separator,
    const String* values,
    size_t value_count)
{
    if(value_count == 0)
        return true;

    assert(str);
    assert(separator);
    assert(values);

    // Put the joined values in a temporary variable to avoid
    // altering the result string if the operation fails.

    // Append the temp string to the result (str) at the end
    // to finalize the operation.
    bool result = false;
    String temp = string_create("");

    if(!string_append_string(&temp, values))
        goto cleanup;

    for(size_t i = 1; i < value_count; i++) {
        if(!string_append_string(&temp, separator))
            goto cleanup;

        if(!string_append_string(&temp, values + i))
            goto cleanup;
    }

    if(!string_append_string(str, &temp))
        goto cleanup;

    result = true;

    cleanup:
        string_free_resources(&temp);

        return result;
}

SSO_STRING_EXPORT bool string_join_refs(
    String* str,
    const String* separator,
    const String** values,
    size_t value_count)
{
    if(value_count == 0)
        return true;
    
    assert(str);
    assert(separator);
    assert(values);

    // Put the joined values in a temporary variable to avoid
    // altering the result string if the operation fails.

    // Append the temp string to the result (str) at the end
    // to finalize the operation.
    bool result = false;
    String temp = string_create("");

    if(!string_append_string(&temp, values[0]))
        goto cleanup;

    for(size_t i = 1; i < value_count; i++) {
        if(!string_append_string(&temp, separator))
            goto cleanup;

        if(!string_append_string(&temp, values[i]))
            goto cleanup;
    }

    if(!string_append_string(str, &temp))
        goto cleanup;

    result = true;

    cleanup:
        string_free_resources(&temp);

        return result;
}

SSO_STRING_EXPORT String* string_split(
    const String* str,
    const String* separator,
    String* results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool init_results)
{
    // Make sure the required inputs aren't NULL.
    assert(str);
    assert(separator);

    // results only needs to be initialized when results_count is > 0.
    // On 0, it doesn't matter what state it is.
    // On a negative number, results will be allocated by the function.
    if(results_count > 0)
        assert(results);

    // Nothing needs to happen if there is no room for results. Return immediately.
    if(results_count == 0) {
        *results_filled = 0;
        return results;
    }

    // Determine if the results array needs to be allocated. If so, create an
    // initial buffer that has space for two items. This is the starting space since
    // splitting small items tends to be pretty common (e.g. an array of key-value pairs).
    bool allocate_results = results_count < 0;
    if(allocate_results) {
        results_count = 2;
        results = malloc(results_count * sizeof(*results));
        if(!results)
            return NULL;

        // The results always have to be initialized by this function if the results array 
        // was also created by it. UB otherwise.
        init_results = true;
    }

    int count = 0;
    size_t start = 0;
    size_t size = string_size(str);

    while(true) {
        // Find the next instance of the separator. If the index is SIZE_MAX,
        // the function reached the end of the string without finding it.
        size_t next = string_find_string(str, start, separator);
        if(next == SIZE_MAX)
            next = size;

        size_t copy_length = next - start;
        if(copy_length != 0 || !skip_empty) {
            // Add the string segment to the results array. If the value fails to be added,
            // there was an allocation error, so just break out of the function right away.
            if(init_results) {
                if(!string_init_size(results + count, string_data(str) + start, copy_length))
                    goto error;
            } else {
                if(!string_append_string_part(results + count, str, start, copy_length))
                    goto error;
            }

            if(++count == results_count) {
                if(allocate_results) {
                    results_count *= 2;
                    void* array = realloc(results, results_count * sizeof(*results));
                    if(!array)
                        goto error;

                    results = array;
                } else {
                    *results_filled = count;
                    return results;
                }
            }
        }

        start = next + string_size(separator);

        // This indicates that the end of the string has been reached, and 
        // that all relevant substrings have been copied into the results array.
        if(start >= size) {
            *results_filled = count;
            return results;
        }
    }

    error:
        if(allocate_results)
            free(results);

        *results_filled = 0;
        return NULL;
}

SSO_STRING_EXPORT String** string_split_refs(
    const String* str,
    const String* separator,
    String** results,
    int results_count,
    int* results_filled,
    bool skip_empty,
    bool init_results) 
{
    // Make sure the required inputs aren't NULL.
    assert(str);
    assert(separator);

    // results only needs to be initialized when results_count is > 0.
    // On 0, it doesn't matter what state it is.
    // On a negative number, results will be allocated by the function.
    if(results_count > 0)
        assert(results);

    // Nothing needs to happen if there is no room for results. Return immediately.
    if(results_count == 0) {
        *results_filled = 0;
        return results;
    }

    // Determine if the results array needs to be allocated. If so, create an
    // initial buffer that has space for two items. This is the starting space since
    // splitting small items tends to be pretty common (e.g. an array of key-value pairs).
    bool allocate_results = results_count < 0;
    if(allocate_results) {
        results_count = 2;
        results = malloc(results_count * sizeof(*results));
        if(!results)
            return NULL;

        // The results always have to be initialized by this function if the results array 
        // was also created by it. UB otherwise.
        init_results = true;
    }

    int count = 0;
    size_t start = 0;
    size_t size = string_size(str);

    while(true) {
        // Find the next instance of the separator. If the index is SIZE_MAX,
        // the function reached the end of the string without finding it.
        size_t next = string_find_string(str, start, separator);
        if(next == SIZE_MAX)
            next = size;

        size_t copy_length = next - start;
        if(copy_length != 0 || !skip_empty) {
            // Add the string segment to the results array. If the value fails to be added,
            // there was an allocation error, so just break out of the function right away.
            if(init_results) {
                String* slice = string_create_ref("");
                if(!slice)
                    goto error;
                results[count] = slice;
            }

            if(!string_append_string_part(results[count], str, start, copy_length))
                goto error;

            if(++count == results_count) {
                if(allocate_results) {
                    results_count *= 2;
                    void* array = realloc(results, results_count * sizeof(*results));
                    if(!array)
                        goto error;

                    results = array;
                } else {
                    *results_filled = count;
                    return results;
                }
            }
        }

        start = next + string_size(separator);

        // This indicates that the end of the string has been reached, and 
        // that all relevant substrings have been copied into the results array.
        if(start >= size) {
            *results_filled = count;
            return results;
        }
    }

    error:
        if(allocate_results) {
            for(size_t i = 0; i < count; i++)
                string_free(results[i]);

            free(results);
        }

        *results_filled = 0;
        return NULL;
}

SSO_STRING_EXPORT size_t ___sso_string_find_impl(const String* str, size_t pos, const char* value, size_t length) {
    assert(str);
    assert(value);
    assert(pos < string_size(str));

    if(pos + length > string_size(str))
        return SIZE_MAX;

    const char* data = string_data(str);
    char* result = strstr(data + pos, value);
    if(result == NULL)
        return SIZE_MAX;
    return result - data;
}

SSO_STRING_EXPORT size_t ___sso_string_find_substr_impl(const String* str, size_t pos, const char* value, size_t length) {
    assert(str);
    assert(value);

    // If the length of the find string is bigger than the search string,
    // early exit.
    size_t size = string_size(str);
    if(pos + length > size)
        return SIZE_MAX;

    size -= length;

    const char* data = string_data(str);
    for(size_t i = pos; i < size; i++) {
        if(strncmp(data + i, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

SSO_STRING_EXPORT size_t ___sso_string_rfind_impl(const String* str, size_t pos, const char* value, size_t length) {
    assert(str);
    assert(value);

    // If the length of the find string is bigger than the search string,
    // early exit.
    size_t size = string_size(str);

    if(pos > size || length > size)
        return SIZE_MAX;

    if(pos < length)
        pos = length;

    // Turn the negative offset into a normal offset.
    pos = size - pos;

    const char* data = string_data(str);

    do {
        if (strncmp(data + pos, value, length) == 0)
            return pos;
    } 
    while (pos-- != 0);

    return SIZE_MAX;
}

SSO_STRING_EXPORT void string_erase(String* str, size_t index, size_t count) {
    assert(str);

    size_t current_size = string_size(str);
    assert(index + count <= current_size);
    char* data = string_cstr(str);
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

SSO_STRING_EXPORT bool string_push_back(String* str, char value) {
    assert(str);

    size_t size;
    char* data;

    if(___sso_string_is_long(str)) {
        size = ___sso_string_long_size(str) + 1;
        if(!___sso_string_long_reserve(str, size))
            return false;
        ___sso_string_long_set_size(str, size);
        data = str->l.data;
    } else {
        size = ___sso_string_short_size(str) + 1;
        switch(___sso_string_short_reserve(str, size)) {
            case ___SSO_STRING_SHORT_RESERVE_RESIZE:
                ___sso_string_long_set_size(str, size);
                data = str->l.data;
                break;
            case ___SSO_STRING_SHORT_RESERVE_SUCCEED:
                ___sso_string_short_set_size(str, size);
                data = str->s.data;
                break;
            case ___SSO_STRING_SHORT_RESERVE_FAIL:
                return false;
        }
    }
    data[size-1] = value;
    data[size] = 0;
    return true;
}

SSO_STRING_EXPORT bool string_u8_push_back(String* str, Char32 value) {
    assert(str);

    size_t size = string_size(str);
    char* data;
    
    if(value < 0x80) {
        if(!string_reserve(str, size + 1))
            return false;
        data = string_cstr(str);
        data[size++] = (char)value;
    } else if(value < 0x800) {
        if(!string_reserve(str, size + 2))
            return false;

        data = string_cstr(str);
        data[size++]     = 0xC0 | ((value >> 6) & 0x1F);
        data[size++] = 0x80 | (value & 0x3f);

    } else if(value < 0x10000) {
        if(!string_reserve(str, size + 3))
            return false;

        data = string_cstr(str);

        data[size++]     = 0xE0 | ((value >> 12) & 0xF);
        data[size++] = 0x80 | ((value >> 6)  & 0x3F);
        data[size++] = 0x80 | (value & 0x3F);
    } else {
        assert(value <= 0x10FFFF);
        if(!string_reserve(str, size + 4))
            return false;

        data = string_cstr(str);

        data[size++]     = 0xF0 | ((value >> 18) & 0x7);
        data[size++] = 0x80 | ((value >> 12) & 0x3F);
        data[size++] = 0x80 | ((value >> 6)  & 0x3F);
        data[size++] = 0x80 | (value & 0x3F);
    }

    ___sso_string_set_size(str, size);
    data[size] = '\0';
    return true;
}

SSO_STRING_EXPORT char string_pop_back(String* str) {
    assert(str);

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
    char* data = string_cstr(str);
    char result = data[size];
    data[size] = 0;
    return result;
}

SSO_STRING_EXPORT Char32 string_u8_pop_back(String* str) {
    assert(str);

    size_t size = string_size(str);
    if(size == 0)
        return 0;

    char* data = string_cstr(str);
    int shift = 0;
    Char32 result = 0;

    unsigned char part = (unsigned char)data[--size];
    while((part & 0xC0) == 0x80) {
        unsigned int stripped_part = (~0x80 & part);
        unsigned int shifted = (stripped_part << (6 * shift++));
        result |= shifted;
        part = (unsigned char)data[--size];
    }

    if(part >= 0xF0)
        result |= (part & 0x07) << 18;
    else if(part >= 0xE0)
        result |= (part & 0x0F) << 12;
    else if(part >= 0xC0)
        result |= (part & 0x1F) << 6;
    else
        result |= part;

    ___sso_string_set_size(str, size);
    data[size] = 0;

    return result;
}

SSO_STRING_EXPORT bool string_resize(String* str, size_t count, char ch) {
    assert(str);
    assert(count < STRING_MAX);
    
    if(!string_reserve(str, count))
        return false;

    char* data = string_cstr(str);

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
    return true;
}

SSO_STRING_EXPORT String* string_format_string(String* result, const String* format, ...) {
    va_list argp;

    va_start(argp, format);

    result = string_format_args_cstr(result, string_data(format), argp);

    va_end(argp);

    return result;
}

SSO_STRING_EXPORT String* string_format_cstr(String* result, const char* format, ...) {
    va_list argp;

    va_start(argp, format);

    result = string_format_args_cstr(result, format, argp);

    va_end(argp);

    return result;
}

SSO_STRING_EXPORT String* string_format_args_string(String* result, const String* format, va_list argp) {
    return string_format_args_cstr(result, string_data(format), argp);
}

SSO_STRING_EXPORT String* string_format_args_cstr(String* result, const char* format, va_list argp) {
    #define ___SSO_STRING_FORMAT_BUFFER_SIZE 256

    #ifdef SSO_THREAD_LOCAL

    static SSO_THREAD_LOCAL char buffer[___SSO_STRING_FORMAT_BUFFER_SIZE];

    #else

    char buffer[___SSO_STRING_FORMAT_BUFFER_SIZE];

    #endif

    size_t original_size = SIZE_MAX;
    if(!result) {
        result = string_create_ref("");
        if(!result)
            return NULL;
        
    } else {
        original_size = string_size(result);
    }

    // argp may have to be used again (if the result of vsnprint is > 256), so
    // make a copy of it to be used with the initial format.
    va_list copy;
    va_copy(copy, argp);

    int written = vsnprintf(buffer, ___SSO_STRING_FORMAT_BUFFER_SIZE, format, copy);
    va_end(copy);

    if(written < 0) {
        goto error;
    } else if(written < ___SSO_STRING_FORMAT_BUFFER_SIZE) {
        if(!string_append_cstr(result, buffer))\
            goto error;
        return result;
    } else {
        if(!string_reserve(result, string_size(result) + written))
            goto error;

        written = vsnprintf(
            string_cstr(result),
            written + 1,
            format,
            argp);

        if(written < 0)
            goto error;

        size_t new_size = original_size == SIZE_MAX ? written : original_size + written;

        // The string should always be long at this point, but do the check just in case.
        if(___sso_string_is_long(result))
            ___sso_string_long_set_size(result, new_size);
        else
            ___sso_string_short_set_size(result, new_size);
    }

    return result;

    error:
        if(original_size == SIZE_MAX)
            string_free_resources(result);
        else
            string_cstr(result)[original_size] = '\0';

        return NULL;
}