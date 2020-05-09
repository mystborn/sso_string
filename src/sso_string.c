#include <sso_string.h>

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

void ___sso_string_long_reserve(String* str, size_t reserve) {
    if(reserve <= ___sso_string_long_cap(str))
        return;

    reserve = ___sso_string_next_cap(0, reserve);
    str->l.data = realloc(str->l.data, reserve + 1);
    str->l.data[reserve] = 0;

    ___sso_string_long_set_cap(str, reserve);
}

// returns true if it had to switch to a large string.
bool ___sso_string_short_reserve(String* str, size_t reserve) {
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

void ___sso_string_ensure_capacity(String* str, size_t cap) {
    size_t current_size = string_capacity(str);
    if(cap <= current_size)
        return;
    cap = ___sso_string_next_cap(current_size, cap);
    string_reserve(str, cap);
}

void ___sso_string_insert_impl(String* str, const char* value, size_t length, size_t index) {
    size_t current_size = string_size(str);
    assert(current_size + length < STRING_MAX);
    ___sso_string_ensure_capacity(str, current_size + length);
    char* data = string_cstr(str);
    data[current_size + length] = 0;
    if(index != current_size)
        memmove(data + index + length, data + index, current_size - index);
    memmove(data + index, value, length);
    ___sso_string_set_size(str, current_size + length);
}

void ___sso_string_append_impl(String* str, const char* value, size_t length) {
    size_t size = string_size(str);
    ___sso_string_ensure_capacity(str, size + length);
    char* data = string_cstr(str);
    memmove(data + size, value, length);
    data[size + length] = 0;
    ___sso_string_set_size(str, size + length);
}

void ___sso_string_replace_impl(String* str, size_t pos, size_t count, const char* value, size_t length) {
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

void string_push_back(String* str, char value) {
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

void string_resize(String* str, size_t count, char ch) {
    assert(count < STRING_MAX);
    ___sso_string_ensure_capacity(str, count);
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
}