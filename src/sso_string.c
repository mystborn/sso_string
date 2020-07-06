#include <sso_string.h>

#define U8_SINGLE 0x7F
#define U8_DOUBLE 0xE0
#define U8_TRIPLE 0xF0

static inline void ___sso_string_set_size(String* str, size_t size) {
    if(___sso_string_is_long(str))
        ___sso_string_long_set_size(str, size);
    else
        ___sso_string_short_set_size(str, size);
}

bool string_init(String* str, const char* cstr) {
    size_t len = strlen(cstr);
    if (len < ___sso_string_min_cap) {
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

bool string_init_size(String* str, const char* cstr, size_t len) {
    assert(len <= strlen(cstr));
    if (len < ___sso_string_min_cap) {
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

Char32 string_u8_get(const String* str, size_t index) {
    size_t size = string_size(str);
    if(index >= size)
        return '\0';

    const char* data = string_data(str) + index;

    if((unsigned char)*data  <= U8_SINGLE)
        return *data;
    else if((unsigned char)*data < U8_DOUBLE)
        return ((*data) << 8) | *(data + 1);
    else if((unsigned char)*data < U8_TRIPLE)
        return ((*data) << 16) | (*(data + 1) << 8) | *(data + 2);
    else
        return ((*data) << 24) | (*(data + 1) << 16) | (*(data + 2) << 8) | *(data + 3);
}

Char32 string_u8_get_with_size(const String* str, size_t index, int* out_size) {
    if(!out_size)
        return string_u8_get(str, index);

    size_t size = string_size(str);
    if(index >= size) {
        *out_size = 0;
        return '\0';
    }

    const char* data = string_data(str) + index;

    if((unsigned char)*data  <= U8_SINGLE) {
        *out_size = 1;
        return *data;
    } else if((unsigned char)*data < U8_DOUBLE) {
        *out_size = 2;
        return ((*data) << 8) | *(data + 1);
    } else if((unsigned char)*data < U8_TRIPLE) {
        *out_size = 3;
        return ((*data) << 16) | (*(data + 1) << 8) | *(data + 2);
    } else {
        *out_size = 4;
        return ((*data) << 24) | (*(data + 1) << 16) | (*(data + 2) << 8) | *(data + 3);
    }
}

int string_u8_codepoint_size(const String* str, size_t index) {
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

void string_shrink_to_fit(String* str) {
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

void string_clear(String* str) {
    if(___sso_string_is_long(str)) {
        str->l.data[0] = 0;
        ___sso_string_long_set_size(str, 0);
    } else {
        str->s.data[0] = 0;
        ___sso_string_short_set_size(str, 0);
    }
}

bool ___sso_string_long_reserve(String* str, size_t reserve) {
    if(reserve <= ___sso_string_long_cap(str))
        return true;

    reserve = ___sso_string_next_cap(0, reserve);
    void* buffer = realloc(str->l.data, reserve + 1);
    if(!buffer)
        return false;

    str->l.data = buffer;
    str->l.data[reserve] = 0;

    ___sso_string_long_set_cap(str, reserve);
    return true;
}

// returns true if it had to switch to a large string.
int ___sso_string_short_reserve(String* str, size_t reserve) {
    if(reserve < ___sso_string_min_cap)
        return ___SSO_STRING_SHORT_RESERVE_SUCCEED;

    reserve = ___sso_string_next_cap(0, reserve);
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

bool ___sso_string_ensure_capacity(String* str, size_t cap) {
    size_t current_size = string_capacity(str);
    if(cap < current_size)
        return true;
    cap = ___sso_string_next_cap(current_size, cap);
    return string_reserve(str, cap);
}

bool ___sso_string_insert_impl(String* str, const char* value, size_t length, size_t index) {
    size_t current_size = string_size(str);
    assert(current_size + length < STRING_MAX);
    if(!___sso_string_ensure_capacity(str, current_size + length))
        return false;
    char* data = string_cstr(str);
    data[current_size + length] = 0;
    if(index != current_size)
        memmove(data + index + length, data + index, current_size - index);
    memmove(data + index, value, length);
    ___sso_string_set_size(str, current_size + length);
    return true;
}

bool ___sso_string_append_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    if(!___sso_string_ensure_capacity(str, size + length))
        return false;
    char* data = string_cstr(str);
    memmove(data + size, value, length);
    data[size + length] = 0;
    ___sso_string_set_size(str, size + length);
    return true;
}

bool ___sso_string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length) {
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

        if(!___sso_string_ensure_capacity(str, size + offset))
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

bool string_u8_is_null_or_whitespace(const String* str) {
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

void string_reverse_bytes(String* str) {
    size_t size = string_size(str);
    char* start = string_cstr(str);
    char* end = start + size - 1;

    while(start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
}

void string_u8_reverse_codepoints(String* str) {
    size_t size = string_size(str);
    char* data;
    char* start = data = string_cstr(str);
    char* end = start + size - 1;
    char tmp;

    // Reverse string bytewise
    while(start < end) {
        tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }

    start = data;

    // Fix up utf8 characters
    // Code adapted from here:
    // https://stackoverflow.com/a/198264
    for(size_t i = size - 1; i >= 0; --i) {
        data = start + i;
        switch((*data & 0xF0) >> 4) {
            case 0xC:
            case 0XD:
                tmp = *data;
                *(data) = *(data - 1);
                *(data - 1) = tmp;
                i--;
                break;
            case 0xE:
                tmp = *data;
                *(data) = *(data - 2);
                *(data - 2) = tmp;
                i -= 2;
                break;
            case 0xF:
                tmp = *data;
                *(data) = *(data - 3);
                *(data - 3) = tmp;
                tmp = *(data - 1);
                *(data - 1) = *(data - 2);
                *(data - 2) = tmp;
                i -= 3;
                break;
            default: 
                break;
        }
    }
}

bool string_join(
    String* str, 
    const String* separator,
    const String* values,
    size_t value_count)
{
    if(value_count == 0)
        return true;

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

bool string_join_refs(
    String* str,
    const String* separator,
    const String** values,
    size_t value_count)
{
    if(value_count == 0)
        return true;

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

int string_split(
    const String* str,
    const String* seperator,
    String* results,
    int results_count,
    bool skip_empty,
    bool init_results)
{
    if(results_count <= 0)
        return 0;

    size_t start = 0;
    int count = 0;
    while(true) {
        size_t next = string_find_string(str, start, seperator);
        if(start == SIZE_MAX)
            return count;

        size_t offset = next - start;
        if(offset != 0 || !skip_empty) {
            if(init_results) {
                if(!string_init_size(results + count, string_data(str) + start, offset))
                    return -1;
            } else {
                if(!string_append_string_part(results + count, str, start, offset))
                    return -1;
            }

            start = next + string_size(seperator);

            if(++count == results_count)
                return count;
        }
    }

    return count;
}

int string_split_refs(
    const String* str,
    const String* seperator,
    String** results,
    int results_count,
    bool skip_empty,
    bool init_results)
{
    if(results_count <= 0)
        return 0;

    size_t start = 0;
    int count = 0;
    while(true) {
        size_t next = string_find_string(str, start, seperator);
        if(start == SIZE_MAX)
            return count;

        size_t offset = next - start;
        if(offset != 0 || !skip_empty) {
            if(init_results) {
                if(!string_init_size(results[count], string_data(str) + start, offset))
                    return -1;
            } else {
                if(!string_append_string_part(results[count], str, start, offset))
                    return -1;
            }

            start = next + string_size(seperator);

            if(++count == results_count)
                return count;
        }
    }

    return count;
}

size_t ___sso_string_find_impl(const String* str, size_t pos, const char* value) {
    assert(pos < string_size(str));
    const char* data = string_data(str);
    char* result = strstr(data + pos, value);
    if(result == NULL)
        return SIZE_MAX;
    return result - data;
}

size_t ___sso_string_find_substr_impl(const String* str, size_t pos, const char* value, size_t length) {
    size_t size = string_size(str) - length;
    assert(pos < size);
    const char* data = string_data(str);
    for(size_t i = pos; i < size; i++) {
        if(strncmp(data + i, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

size_t ___sso_string_rfind_impl(const String* str, size_t pos, const char* value, size_t length) {
    assert(pos < string_size(str));
    const char* data = string_data(str);
    size_t i = pos + 1;
    
    while(i != 0) {
        if(strncmp(data + --i, value, length) == 0)
            return i;
    }

    return SIZE_MAX;
}

void string_erase(String* str, size_t index, size_t count) {
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

bool string_push_back(String* str, char value) {
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

bool string_u8_push_back(String* str, Char32 value) {
    size_t size = string_size(str);
    char* data;

    if(value >> 24) {
        if(!string_reserve(str, size + 4))
            return false;
        data = string_cstr(str);
        data[size++] = (char)(value >> 24) & 0xFF;
        data[size++] = (char)(value >> 16) & 0xFF;
        data[size++] = (char)(value >> 8) & 0xFF;
        data[size] = (char)(value) & 0xFF;
    } else if(value >> 16) {
        if(!string_reserve(str, size + 3))
            return false;
        data = string_cstr(str);
        data[size++] = (char)(value >> 16) & 0xFF;
        data[size++] = (char)(value >> 8) & 0xFF;
        data[size] = (char)(value) & 0xFF;
    } else if(value >> 8) {
        if(!string_reserve(str, size + 2))
            return false;
        data = string_cstr(str);
        data[size++] = (char)(value >> 8) & 0xFF;
        data[size] = (char)(value) & 0xFF;
    } else {
        if(!string_reserve(str, size + 1))
            return false;
        data = string_cstr(str);
        data[size] = (char)(value) & 0xFF;
    }
    data[size + 1] = '\0';
    return true;
}

char string_pop_back(String* str) {
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

Char32 string_u8_pop_back(String* str) {
    size_t size = string_size(str);
    if(size == 0)
        return 0;

    const char* data = string_data(str);

    Char32 result = data[--size];
    int shift = 1;
    while(size > 0 && data[--size] > U8_SINGLE)
        result |= (data[size] << (8 * shift));

    return result;
}

bool string_resize(String* str, size_t count, char ch) {
    assert(count < STRING_MAX);
    if(!___sso_string_ensure_capacity(str, count))
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