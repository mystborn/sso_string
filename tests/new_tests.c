#include <check.h>

#include <stdio.h>
#include <locale.h>

#include "../include/sso_string.h"

static String small;
static String large;

#define HELLO "hello"
#define HELLO_SIZE 5
#define KANA "こんいちは"
#define KANA_SIZE 15
#define KANA_WIDE L"\u3053\u3093\u3044\u3061\x306F";
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_SIZE 20
#define MIXED "hこeんlいlちoは"
#define MIXED_SIZE 20
#define ALL_CODEPOINTS { 0x0001, 0x007F, 0x00080, 0x007FF, 0x00800, 0x0FFFF, 0x10000, 0x10FFFF }
#define ALL_CODEPOINTS_SIZE 20

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static void string_start(void) {
    string_init(&small, "hello");
    string_init(&large, ALPHABET);
}

static void string_reset(void) {
    string_free_resources(&small);
    string_free_resources(&large);
}

START_TEST(string_small_has_small_flag) {
    String str = string_create("");
    ck_assert(!___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_long_has_long_flag) {
    String str = string_create(ALPHABET);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_switches_size) {
    // This test not only checks that the string
    // switches size correctly, but also that
    // the capacity behaviour is the same for small strings
    // and large strings.

    String str = string_create("");

    int cap = string_capacity(&str);

    while(string_size(&str) <= cap) {
        ck_assert(!___sso_string_is_long(&str));
        string_push_back(&str, 'a');
    }

    ck_assert(___sso_string_is_long(&str));

    cap = string_capacity(&str);

    while(string_size(&str) <= cap) {
        ck_assert(string_capacity(&str) == cap);
        string_push_back(&str, 'b');
    }

    ck_assert(string_capacity(&str) != cap);

    cap = string_capacity(&str);

    while(string_size(&str) <= cap) {
        ck_assert(string_capacity(&str) == cap);
        string_push_back(&str, 'c');
    }

    ck_assert(string_capacity(&str) != cap);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_is_correct_size) {
    // This test just makes sure that the string does not take up
    // any more space with the short string optimization than it
    // would with a naive implementation
    ck_assert(sizeof(String) == sizeof(size_t) * 2 + sizeof(char*));
}
END_TEST

START_TEST(string_init_copies_cstr) {
    char value[] = "moo";
    String str = string_create(value);

    ck_assert(value != string_data(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_small_data) {
    ck_assert(strcmp(string_data(&small), HELLO) == 0);
} 
END_TEST

START_TEST(string_large_data) {
    ck_assert(strcmp(string_data(&large), ALPHABET) == 0);
} 
END_TEST

START_TEST(string_small_cstr) {
    ck_assert(strcmp(string_cstr(&small), HELLO) == 0);

    // The reference to cstr and data should be the same
    // but data is an immutable view.
    ck_assert(string_cstr(&small) == string_data(&small));
}
END_TEST

START_TEST(string_large_cstr) {
    ck_assert(strcmp(string_cstr(&large), ALPHABET) == 0);
    
    // The reference to cstr and data should be the same
    // but data is an immutable view.
    ck_assert(string_cstr(&large) == string_data(&large));
}
END_TEST

START_TEST(string_small_size) {
    ck_assert(string_size(&small) == strlen(HELLO));
}
END_TEST

START_TEST(string_large_size) {
    ck_assert(string_size(&large) == strlen(ALPHABET));
}
END_TEST

START_TEST(string_size_dynamic) {
    String str = string_create("");
    for(int i = 0; i < 100; i++) {
        ck_assert(string_size(&str) == i);
        string_push_back(&str, 'a');
    }
    string_free_resources(&str);
}
END_TEST

START_TEST(string_size_utf8) {
    String str = string_create(KANA);
    ck_assert(string_size(&str) == KANA_SIZE);
    ck_assert(string_size(&str) == strlen(string_data(&str)));

    string_free_resources(&str);


    string_init(&str, "");
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        string_u8_push_back(&str, all_codepoints[i]);

    ck_assert(string_size(&str) == ALL_CODEPOINTS_SIZE);
    ck_assert(string_size(&str) == strlen(string_data(&str)));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_codepoints_ascii) {
    ck_assert(string_u8_codepoints(&small) == strlen(HELLO));
}
END_TEST

START_TEST(string_u8_codepoints_utf8) {
    String str = string_create(KANA);
    ck_assert(string_u8_codepoints(&str) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_codepoints_all) {
    String str = string_create("");
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        string_u8_push_back(&str, all_codepoints[i]);

    ck_assert(string_u8_codepoints(&str) == ARRAY_SIZE(all_codepoints));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_get_all) {
    String kana = string_create(KANA);
    String strings[3] = { small, large, kana };
    for(int i = 0; i < 3; i++) {
        String* str = strings + i;
        const char* data = string_data(str);
        for(int j = 0; j < string_size(str); j++)
            ck_assert(string_get(str, j) == data[j]);
    }
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_u8_get_ascii) {
    String strings[2] = { small, large };
    for(int i = 2; i < 2; i++) {
        String* str = strings + i;
        const char* data = string_data(str);
        for(int j = 0; j < string_size(str); j++)
            ck_assert(string_get(str, j) == data[j]);
    }
}
END_TEST

START_TEST(string_u8_get_utf8) {
    String str = string_create(KANA);
    wchar_t wide[] = KANA_WIDE;
    for(int i = 0; i * 3 < string_size(&str); i++) {
        Char32 kana = string_u8_get(&str, i * 3);
        ck_assert(kana == wide[i]);
    }
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_get_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = string_create("");
    
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push_back(&str, all_codepoints[i]));

    int size = 1;
    int index = 0;
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++) {
        Char32 character = string_u8_get(&str, index);
        index += size;
        if(i % 2 == 1)
            size++;
    }
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_get_with_size_ascii) {
    String strings[2] = { small, large };
    for(int i = 2; i < 2; i++) {
        String* str = strings + i;
        const char* data = string_data(str);
        int index = 0;
        int size;
        Char32 current;
        do {
            current = string_u8_get_with_size(str, index, &size);
            ck_assert(current == data[index]);
            index += size;
        }
        while(current);
    }
}
END_TEST

START_TEST(string_u8_get_with_size_utf8) {
    String str = string_create(KANA);
    wchar_t wide[] = KANA_WIDE;
    int index = 0;
    int size;
    Char32 current;
    do {
        current = string_u8_get_with_size(&str, index, &size);
        ck_assert(current == wide[index / 3]);
        index += size;
    }
    while(current);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_get_with_size_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = string_create("");
    
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push_back(&str, all_codepoints[i]));

    int index = 0;
    int count = 0;
    int expected_size = 1;
    int size;
    Char32 current;
    do {
        current = string_u8_get_with_size(&str, index, &size);
        index += size;
        if(current) {
            ck_assert(current == all_codepoints[count]);
            ck_assert(size == expected_size);
            count++;
            if(count % 2 == 0)
                expected_size++;
        }
    }
    while(current);

    ck_assert(count == ARRAY_SIZE(all_codepoints));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_codepoint_size_ascii) {
    String strings[2] = { small, large };
    for(int i = 2; i < 2; i++) {
        String* str = strings + i;
        int index = 0;
        int count = 0;
        int size;
        do {
            size = string_u8_codepoint_size(str, index);
            ck_assert(size == 1);
            index += size;
            if(size)
                count++;
        }
        while(size);

        ck_assert(count == string_u8_codepoints(str));
    }
}
END_TEST

START_TEST(string_u8_codepoint_size_utf8) {
    String str = string_create(KANA);
    int index = 0;
    int count = 0;
    int size;
    do {
        size = string_u8_codepoint_size(&str, index);
        index += size;
        if(size) {
            ck_assert(size == 3);
            count++;
        }
    }
    while(size);

    ck_assert(count == string_u8_codepoints(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_codepoint_size_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = string_create("");
    
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push_back(&str, all_codepoints[i]));

    int index = 0;
    int count = 0;
    int expected_size = 1;
    int size;
    do {
        size = string_u8_codepoint_size(&str, index);
        index += size;
        if(size) {
            ck_assert(size == expected_size);
            count++;
            if(count % 2 == 0)
                expected_size++;
        }
    }
    while(size);

    ck_assert(count == string_u8_codepoints(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_set_ascii) {
    char* start = "abc";
    char* middle = "aba";
    char* end = "cba";

    String str = string_create(start);

    string_set(&str, 2, 'a');
    ck_assert(string_get(&str, 2) == 'a');
    ck_assert(strcmp(string_data(&str), middle) == 0);

    string_set(&str, 0, 'c');
    ck_assert(string_get(&str, 0) == 'c');
    ck_assert(strcmp(string_data(&str), end) == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_set_mixed) {
    char* start = "aんb";
    char* middle = "yんb";
    char* end = "yんz";

    String str = string_create(start);
    ck_assert(string_size(&str) == 5);

    string_set(&str, 0, 'y');
    ck_assert(string_get(&str, 0) == 'y');
    ck_assert(strcmp(string_data(&str), middle) == 0);

    string_set(&str, 4, 'z');
    ck_assert(string_get(&str, 4) == 'z');
    ck_assert(strcmp(string_data(&str), end) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_set_ascii) {
    char* start = "abc";
    char* middle = "aba";
    char* end = "cba";

    String str = string_create(start);

    string_u8_set(&str, 2, 'a');
    ck_assert(string_u8_get(&str, 2) == 'a');
    ck_assert(strcmp(string_data(&str), middle) == 0);

    string_u8_set(&str, 0, 'c');
    ck_assert(string_u8_get(&str, 0) == 'c');
    ck_assert(strcmp(string_data(&str), end) == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_set_smaller) {
    char* expected = "こnいちは";

    String str = string_create(KANA);
    string_u8_set(&str, 3, 'n');
    ck_assert(string_size(&str) == 13);
    ck_assert(string_u8_get(&str, 3) == 'n');
    ck_assert(strcmp(string_data(&str), expected) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_set_bigger) {
    char* expected = "koんnichiwa";

    String str = string_create("konnichiwa");
    string_u8_set(&str, 2, L'\u3093');
    ck_assert(string_size(&str) == 12);
    ck_assert(string_u8_get(&str, 2) == L'\u3093');
    ck_assert(strcmp(string_data(&str), expected) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_empty_empty) {
    String str = string_create("");
    ck_assert(string_empty(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_empty_whitespace) {
    String str = string_create(" ");
    ck_assert(!string_empty(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_empty_letters) {
    ck_assert(!string_empty(&small));
    ck_assert(!string_empty(&large));
}
END_TEST

START_TEST(string_reserve_small_less_than_capacity) {
    size_t reserve = string_capacity(&small) - 1;
    ck_assert(string_reserve(&small, reserve));
    ck_assert(string_capacity(&small) > reserve);
}
END_TEST

START_TEST(string_reserve_small_greater_than_capacity) {
    size_t reserve = string_capacity(&small) + 1;
    ck_assert(string_reserve(&small, reserve));
    ck_assert(string_capacity(&small) > reserve);
}
END_TEST

START_TEST(string_reserve_large_less_than_capacity) {
    size_t reserve = string_capacity(&large) - 1;
    ck_assert(string_reserve(&large, reserve));
    ck_assert(string_capacity(&large) > reserve);
}
END_TEST

START_TEST(string_reserve_large_greater_than_capacity) {
    size_t reserve = string_capacity(&large) + 1;
    ck_assert(string_reserve(&large, reserve));
    ck_assert(string_capacity(&large) > reserve);
}
END_TEST

START_TEST(string_shrink_small_to_small) {
    size_t capacity = string_capacity(&small);
    string_shrink_to_fit(&small);
    ck_assert(capacity == string_capacity(&small));
}
END_TEST

START_TEST(string_shrink_large_to_large) {
    if(string_size(&large) == string_capacity(&large))
        string_push_back(&large, 'a');

    ck_assert(string_size(&large) != string_capacity(&large));
    string_shrink_to_fit(&large);
    ck_assert(string_size(&large) == string_capacity(&large));
}
END_TEST

START_TEST(string_shrink_large_to_small) {
    string_clear(&large);
    ck_assert(___sso_string_is_long(&large));
    string_shrink_to_fit(&large);
    ck_assert(!___sso_string_is_long(&large));

    // Make sure the string is functional after switching sizes
    string_append_cstr(&large, HELLO);
    ck_assert(strcmp(string_data(&large), HELLO) == 0);
}
END_TEST

START_TEST(string_clear_small) {
    ck_assert(string_size(&small) != 0);
    size_t capacity = string_capacity(&small);
    string_clear(&small);
    ck_assert(string_size(&small) == 0);
    ck_assert(string_capacity(&small) == capacity);
}
END_TEST

START_TEST(string_clear_large) {
    ck_assert(string_size(&large) != 0);
    size_t capacity = string_capacity(&large);
    string_clear(&large);
    ck_assert(string_size(&large) == 0);
    ck_assert(string_capacity(&large) == capacity);
}
END_TEST

START_TEST(string_insert_cstr_ascii) {
    String str = string_create("lexd");
    ck_assert(string_insert_cstr(&str, "a", 0));
    ck_assert(strcmp(string_data(&str), "alexd") == 0);

    ck_assert(string_insert_cstr(&str, "an", 4));
    ck_assert(strcmp(string_data(&str), "alexand") == 0);

    ck_assert(string_insert_cstr(&str, "er", 7));
    ck_assert(strcmp(string_data(&str), "alexander") == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_cstr_utf8) {
    String str = string_create("んち");

    ck_assert(string_insert_cstr(&str, "こ", 0));
    ck_assert(strcmp(string_data(&str), "こんち") == 0);

    ck_assert(string_insert_cstr(&str, "に", 6));
    ck_assert(strcmp(string_data(&str), "こんにち") == 0);

    ck_assert(string_insert_cstr(&str, "は。", 12));
    ck_assert(strcmp(string_data(&str), "こんにちは。") == 0);

}
END_TEST

START_TEST(string_insert_string_ascii) {
    String str = string_create("lexd");
    String a = string_create("a");
    String an = string_create("an");
    String er = string_create("er");

    ck_assert(string_insert_string(&str, &a, 0));
    ck_assert(strcmp(string_data(&str), "alexd") == 0);

    ck_assert(string_insert_string(&str, &an, 4));
    ck_assert(strcmp(string_data(&str), "alexand") == 0);

    ck_assert(string_insert_string(&str, &er, 7));
    ck_assert(strcmp(string_data(&str), "alexander") == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_string_utf8) {
    String str = string_create("んち");
    String ko = string_create("こ");
    String ni = string_create("に");
    String wa = string_create("は。");

    ck_assert(string_insert_string(&str, &ko, 0));
    ck_assert(strcmp(string_data(&str), "こんち") == 0);

    ck_assert(string_insert_string(&str, &ni, 6));
    ck_assert(strcmp(string_data(&str), "こんにち") == 0);

    ck_assert(string_insert_string(&str, &wa, 12));
    ck_assert(strcmp(string_data(&str), "こんにちは。") == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_cstr_part_ascii) {
    String str = string_create("lexd");
    char* result = "alexander";

    ck_assert(string_insert_cstr_part(&str, result, 0, 0, 1));
    ck_assert(strcmp(string_data(&str), "alexd") == 0);

    ck_assert(string_insert_cstr_part(&str, result, 4, 4, 2));
    ck_assert(strcmp(string_data(&str), "alexand") == 0);

    ck_assert(string_insert_cstr_part(&str, result, 7, 7, 2));
    ck_assert(strcmp(string_data(&str), result) == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_cstr_part_utf8) {
    String str = string_create("んち");
    char* result = "こんにちは。";

    ck_assert(string_insert_cstr_part(&str, result, 0, 0, 3));
    ck_assert(strcmp(string_data(&str), "こんち") == 0);

    ck_assert(string_insert_cstr_part(&str, result, 6, 6, 3));
    ck_assert(strcmp(string_data(&str), "こんにち") == 0);

    ck_assert(string_insert_cstr_part(&str, result, 12, 12, 6));
    ck_assert(strcmp(string_data(&str), "こんにちは。") == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_string_part_ascii) {
    String str = string_create("lexd");
    String result = string_create("alexander");

    ck_assert(string_insert_string_part(&str, &result, 0, 0, 1));
    ck_assert(strcmp(string_data(&str), "alexd") == 0);

    ck_assert(string_insert_string_part(&str, &result, 4, 4, 2));
    ck_assert(strcmp(string_data(&str), "alexand") == 0);

    ck_assert(string_insert_string_part(&str, &result, 7, 7, 2));
    ck_assert(strcmp(string_data(&str), "alexander") == 0);

    string_free_resources(&str);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_insert_string_part_utf8) {
    String str = string_create("んち");
    String result = string_create("こんにちは。");

    ck_assert(string_insert_string_part(&str, &result, 0, 0, 3));
    ck_assert(strcmp(string_data(&str), "こんち") == 0);

    ck_assert(string_insert_string_part(&str, &result, 6, 6, 3));
    ck_assert(strcmp(string_data(&str), "こんにち") == 0);

    ck_assert(string_insert_string_part(&str, &result, 12, 12, 6));
    ck_assert(strcmp(string_data(&str), "こんにちは。") == 0);

    string_free_resources(&str);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_erase_small_start) {
    string_erase(&small, 0, 1);
    ck_assert(string_equals_cstr(&small, "ello"));
}
END_TEST

START_TEST(string_erase_small_end) {
    string_erase(&small, 3, 2);
    ck_assert(string_equals_cstr(&small, "hel"));
}
END_TEST

START_TEST(string_erase_small_middle) {
    string_erase(&small, 1, 3);
    ck_assert(string_equals_cstr(&small, "ho"));
}
END_TEST

START_TEST(string_erase_large_start) {
    string_erase(&large, 0, 1);
    ck_assert(string_equals_cstr(&large, "bcdefghijklmnopqrstuvwxyz"));
}
END_TEST

START_TEST(string_erase_large_end) {
    string_erase(&large, 23, 3);
    ck_assert(string_equals_cstr(&large, "abcdefghijklmnopqrstuvw"));
}
END_TEST

START_TEST(string_erase_large_middle) {
    string_erase(&large, 1, 24);
    ck_assert(string_equals_cstr(&large, "az"));
}
END_TEST

START_TEST(string_push_back_succeeds) {
    String str = string_create("");
    ck_assert(string_push_back(&str, 'h'));
    ck_assert(string_equals_cstr(&str, "h"));

    ck_assert(string_push_back(&str, 'e'));
    ck_assert(string_equals_cstr(&str, "he"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_push_back_ascii) {
    String str = string_create("");
    ck_assert(string_u8_push_back(&str, 'h'));
    ck_assert(string_equals_cstr(&str, "h"));

    ck_assert(string_u8_push_back(&str, 'e'));
    ck_assert(string_equals_cstr(&str, "he"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_push_back_utf8) {
    String str = string_create("");
    wchar_t kana[] = KANA_WIDE;
    string_u8_push_back(&str, kana[0]);
    ck_assert(string_equals_cstr(&str, "こ"));

    string_u8_push_back(&str, kana[1]);
    ck_assert(string_equals_cstr(&str, "こん"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_push_back_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = string_create("");

    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push_back(&str, all_codepoints[i]));

    int index = 0;
    int count = 0;
    int size;
    Char32 current;
    do {
        current = string_u8_get_with_size(&str, index, &size);
        index += size;
        if(current) {
            ck_assert(current == all_codepoints[count]);
            count++;
        }
    }
    while(current);

    ck_assert(count == ARRAY_SIZE(all_codepoints));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_pop_back_succeeds) {
    char* hello = HELLO;
    for(int i = HELLO_SIZE - 1; i >= 0; i--) {
        char letter = string_pop_back(&small);
        ck_assert(letter = hello[i]);
    }
}
END_TEST

START_TEST(string_u8_pop_back_ascii) {
    char* hello = HELLO;
    for(int i = HELLO_SIZE - 1; i >= 0; i--) {
        Char32 letter = string_u8_pop_back(&small);
        ck_assert(letter = hello[i]);
    }
}
END_TEST

START_TEST(string_u8_pop_back_utf8) {
    String str = string_create(KANA);
    String result = string_create(KANA);
    int codepoints = string_u8_codepoints(&result);
    int size = string_size(&str);

    for(int i = codepoints - 1; i >= 0; i--) {
        Char32 kana = string_u8_pop_back(&str);
        Char32 expected = string_u8_get(&result, i * 3);
        ck_assert(kana == expected);
        size_t new_size = string_size(&str);
        ck_assert(new_size == size - 3);
        size = new_size;
    }

    string_free_resources(&str);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_u8_pop_back_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = string_create("");

    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push_back(&str, all_codepoints[i]));

    String result = string_create(string_data(&str));

    int expected_size = 4;
    int offset = 4;
    size_t size = string_size(&result);

    for(int i = ARRAY_SIZE(all_codepoints) - 1; i >= 0; i--) {
        Char32 letter = string_u8_pop_back(&str);
        Char32 expected_letter = string_u8_get(&result, size - offset);

        ck_assert(letter == expected_letter);

        if(i % 2 == 0)
            expected_size--;

        offset += expected_size;
    }

    string_free_resources(&str);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_append_cstr_small_to_small) {
    String str = string_create("hel");
    ck_assert(string_append_cstr(&str, "lo"));
    ck_assert(string_equals_cstr(&str, HELLO));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_cstr_large_to_large) {
    String str = string_create(ALPHABET);
    ck_assert(string_append_cstr(&str, ALPHABET));
    ck_assert(string_equals_cstr(&str, ALPHABET ALPHABET));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_cstr_small_to_large) {
    String str = string_create("");
    ck_assert(!___sso_string_is_long(&str));
    ck_assert(string_append_cstr(&str, ALPHABET));
    ck_assert(string_equals_cstr(&str, ALPHABET));
    ck_assert(___sso_string_is_long(&str));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_string_small_to_small) {
    String str = string_create("hel");
    String value = string_create("lo");
    ck_assert(string_append_string(&str, &value));
    ck_assert(string_equals_cstr(&str, HELLO));

    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_append_string_large_to_large) {
    String str = string_create(ALPHABET);
    String value = string_create(ALPHABET);
    ck_assert(string_append_string(&str, &value));
    ck_assert(string_equals_cstr(&str, ALPHABET ALPHABET));

    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_append_string_small_to_large) {
    String str = string_create("");
    String value = string_create(ALPHABET);
    ck_assert(!___sso_string_is_long(&str));
    ck_assert(string_append_string(&str, &value));
    ck_assert(string_equals_cstr(&str, ALPHABET));
    ck_assert(___sso_string_is_long(&str));

    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_append_cstr_part_succeeds) {
    String str = string_create("hel");
    ck_assert(string_append_cstr_part(&str, "Hello!", 3, 2));
    ck_assert(string_equals_cstr(&str, HELLO));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_string_part_succeeds) {
    String str = string_create("hel");
    String value = string_create("Hello!");
    ck_assert(string_append_string_part(&str, &value, 3, 2));
    ck_assert(string_equals_cstr(&str, HELLO));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_compare_cstr_less_than) {
    String str = string_create("a");
    ck_assert(string_compare_cstr(&str, "b") == -1);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_compare_cstr_greater_than) {
    String str = string_create("b");
    ck_assert(string_compare_cstr(&str, "a") == 1);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_compare_cstr_equals) {
    String str = string_create("a");
    ck_assert(string_compare_cstr(&str, "a") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_compare_string_less_than) {
    String str = string_create("a");
    String value = string_create("b");
    ck_assert(string_compare_string(&str, &value) == -1);
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_compare_string_greater_than) {
    String str = string_create("b");
    String value = string_create("a");
    ck_assert(string_compare_string(&str, &value) == 1);
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_compare_string_equals) {
    String str = string_create("a");
    String value = string_create("a");
    ck_assert(string_compare_string(&str, &value) == 0);
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_equals_cstr_true) {
    String str = string_create(HELLO);
    ck_assert(string_equals_cstr(&str, HELLO));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_equals_cstr_false) {
    String str = string_create(ALPHABET);
    ck_assert(!string_equals_cstr(&str, HELLO));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_equals_string_true) {
    String str = string_create(HELLO);
    String value = string_create(HELLO);
    ck_assert(string_equals_string(&str, &value));
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_equals_string_false) {
    String str = string_create(ALPHABET);
    String value = string_create(HELLO);
    ck_assert(!string_equals_string(&str, &value));
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_starts_with_cstr_true) {
    ck_assert(string_starts_with_cstr(&large, "abc"));
}
END_TEST

START_TEST(string_starts_with_cstr_false) {
    ck_assert(!string_starts_with_cstr(&large, "xyz"));
}
END_TEST

START_TEST(string_starts_with_string_true) {
    String value = string_create("abc");
    ck_assert(string_starts_with_string(&large, &value));
    string_free_resources(&value);
}
END_TEST

START_TEST(string_starts_with_string_false) {
    String value = string_create("xyz");
    ck_assert(!string_starts_with_string(&large, &value));
    string_free_resources(&value);
}
END_TEST

START_TEST(string_ends_with_cstr_true) {
    ck_assert(string_ends_with_cstr(&large, "xyz"));
}
END_TEST

START_TEST(string_ends_with_cstr_false) {
    ck_assert(!string_ends_with_cstr(&large, "abc"));
}
END_TEST

START_TEST(string_ends_with_string_true) {
    String value = string_create("xyz");
    ck_assert(string_ends_with_string(&large, &value));
    string_free_resources(&value);
}
END_TEST

START_TEST(string_ends_with_string_false) {
    String value = string_create("abc");
    ck_assert(!string_ends_with_string(&large, &value));
    string_free_resources(&value);
}
END_TEST

START_TEST(string_replace_cstr_shrink) {
    ck_assert(string_replace_cstr(&small, 1, 3, "b"));
    ck_assert(string_equals(&small, "hbo"));
}
END_TEST

START_TEST(string_replace_cstr_shrink_end) {
    ck_assert(string_replace_cstr(&small, 3, 2, "p"));
    ck_assert(string_equals(&small, "help"));
}
END_TEST

START_TEST(string_replace_cstr_same_size) {
    ck_assert(string_replace_cstr(&small, 2, 1, "n"));
    ck_assert(string_equals(&small, "henlo"));
}
END_TEST

START_TEST(string_replace_cstr_grow_small_to_small) {
    String str = string_create("hbo");
    ck_assert(string_replace_cstr(&str, 1, 1, "ell"));
    ck_assert(string_equals_cstr(&str, HELLO));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_cstr_grow_small_to_large) {
    String str = string_create("Helero");
    char* value = " runs heroically into the burning building like a he";
    ck_assert(string_replace_cstr(&str, 2, 2, value));
    ck_assert(string_equals_cstr(&str, "He runs heroically into the burning building like a hero"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_shrink) {
    String b = string_create("b");
    ck_assert(string_replace_string(&small, 1, 3, &b));
    ck_assert(string_equals(&small, "hbo"));
    string_free_resources(&b);
}
END_TEST

START_TEST(string_replace_string_shrink_end) {
    String p = string_create("p");
    ck_assert(string_replace_string(&small, 3, 2, &p));
    ck_assert(string_equals(&small, "help"));
    string_free_resources(&p);
}
END_TEST

START_TEST(string_replace_string_same_size) {
    String n = string_create("n");
    ck_assert(string_replace_string(&small, 2, 1, &n));
    ck_assert(string_equals(&small, "henlo"));
    string_free_resources(&n);
}
END_TEST

START_TEST(string_replace_string_grow_small_to_small) {
    String ell = string_create("ell");
    String str = string_create("hbo");
    ck_assert(string_replace_string(&str, 1, 1, &ell));
    ck_assert(string_equals(&str, HELLO));
    string_free_resources(&ell);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_grow_small_to_large) {
    String hero = string_create(" runs heroically into the burning building like a he");
    String str = string_create("Helero");
    ck_assert(string_replace_string(&str, 2, 2, &hero));
    ck_assert(string_equals(&str, "He runs heroically into the burning building like a hero"));

    string_free_resources(&str);
    string_free_resources(&hero);
}
END_TEST

START_TEST(string_substring_part) {
    String hell;
    ck_assert(string_substring(&small, 0, 4, &hell));
    ck_assert(string_equals(&hell, "hell"));

    String ello;
    ck_assert(string_substring(&small, 1, 4, &ello));
    ck_assert(string_equals(&ello, "ello"));

    string_free_resources(&hell);
    string_free_resources(&ello);
}
END_TEST

START_TEST(string_substring_whole) {
    String hello;
    ck_assert(string_substring(&small, 0, 5, &hello));
    ck_assert(string_equals_string(&hello, &small));
    string_free_resources(&hello);
}
END_TEST

START_TEST(string_copy_small) {
    String hello;
    ck_assert(string_copy(&small, &hello));
    ck_assert(string_equals_string(&small, &hello));
    string_free_resources(&hello);
}
END_TEST

START_TEST(string_copy_large) {
    String alphabet;
    ck_assert(string_copy(&large, &alphabet));
    ck_assert(string_equals_string(&alphabet, &large));
    string_free_resources(&alphabet);
}
END_TEST

START_TEST(string_copy_to_short) {
    char buffer[6];
    string_copy_to(&small, buffer, 0, 5);
    buffer[5] = '\0';
    ck_assert(string_equals(&small, buffer));
}
END_TEST

START_TEST(string_copy_to_large) {
    char buffer[27];
    string_copy_to(&large, buffer, 0, 26);
    buffer[26] = '\0';
    ck_assert(string_equals(&large, buffer));
}
END_TEST

START_TEST(string_resize_shrink) {
    ck_assert(string_resize(&small, 4, ' '));
    ck_assert(string_equals(&small, "hell"));
    ck_assert(string_size(&small) == 4);
}
END_TEST

START_TEST(string_resize_same_size) {
    size_t old_size = string_size(&small);
    ck_assert(string_resize(&small, 5, ' '));
    ck_assert(string_equals(&small, "hello"));
    ck_assert(string_size(&small) == old_size);
}
END_TEST

START_TEST(string_resize_grow) {
    ck_assert(string_resize(&small, 8, ' '));
    ck_assert(string_equals(&small, "hello   "));
    ck_assert(string_size(&small) == 8);
}
END_TEST

START_TEST(string_swap_small_and_large) {
    size_t small_size = string_size(&small);
    size_t large_size = string_size(&large);
    size_t small_cap = string_capacity(&small);
    size_t large_cap = string_capacity(&large);

    string_swap(&small, &large);

    ck_assert(string_equals(&small, ALPHABET));
    ck_assert(string_equals(&large, HELLO));
    ck_assert(string_size(&small) == large_size);
    ck_assert(string_size(&large) == small_size);
    ck_assert(string_capacity(&small) == large_cap);
    ck_assert(string_capacity(&large) == small_cap);
}
END_TEST

START_TEST(string_find_cstr_first) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_find_cstr(&str, 0, HELLO) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_find_cstr_second) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_find_cstr(&str, 1, HELLO) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_find_cstr_none) {
    ck_assert(string_find_cstr(&small, 0, "moo") == SIZE_MAX);
}
END_TEST

START_TEST(string_find_string_first) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_find_string(&str, 0, &small) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_find_string_second) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_find_string(&str, 1, &small) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_find_string_none) {
    String moo = string_create("moo");
    ck_assert(string_find_string(&small, 0, &moo) == SIZE_MAX);
    string_free_resources(&moo);
}
END_TEST

START_TEST(string_rfind_cstr_first) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_cstr(&str, string_size(&str), HELLO) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_cstr_second) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_cstr(&str, 4, HELLO) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_cstr_none) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_cstr(&str, string_size(&str), "moo") == SIZE_MAX);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_string_first) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_string(&str, string_size(&str), &small) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_string_second) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_string(&str, 4, &small) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_string_none) {
    String str = string_create(HELLO HELLO);
    String moo = string_create("moo");
    ck_assert(string_rfind_string(&str, string_size(&str), &moo) == SIZE_MAX);
    string_free_resources(&str);
    string_free_resources(&moo);
}
END_TEST

START_TEST(string_is_null_or_empty_null) {
    ck_assert(string_is_null_or_empty(NULL));
}
END_TEST

START_TEST(string_is_null_or_empty_empty) {
    String str = string_create("");
    ck_assert(string_is_null_or_empty(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_is_null_or_empty_filled) {
    ck_assert(!string_is_null_or_empty(&small));
}
END_TEST

START_TEST(string_is_null_or_whitespace_null) {
    ck_assert(string_u8_is_null_or_whitespace(NULL));
}
END_TEST

START_TEST(string_is_null_or_whitespace_empty) {
    String str = string_create("");
    ck_assert(string_u8_is_null_or_whitespace(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_is_null_or_whitespace_space) {
    String str = string_create(" ");
    ck_assert(string_u8_is_null_or_whitespace(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_is_null_or_whitespace_u8_space) {
    String str = string_create("");
    string_u8_push_back(&str, ' '); // ascii space
    string_u8_push_back(&str, L'\xA0'); // no-break space
    string_u8_push_back(&str, L'\x180E'); // mongolian vowel separator
    ck_assert(string_u8_is_null_or_whitespace(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_is_null_or_whitespace_filled) {
    String str = string_create(" a ");
    ck_assert(!string_u8_is_null_or_whitespace(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_reverse_bytes_ascii) {
    string_reverse_bytes(&small);
    ck_assert(string_equals(&small, "olleh"));
}
END_TEST

START_TEST(string_reverse_bytes_utf8) {
    char result[] = KANA;
    size_t index = ARRAY_SIZE(result) - 2;
    String kana = string_create(KANA);
    string_reverse_bytes(&kana);

    for(int i = 0; i < string_size(&kana); i++, index--) {
        ck_assert(string_get(&kana, i) == result[index]);
    }

    string_free_resources(&kana);
}
END_TEST

START_TEST(string_u8_reverse_codepoints_ascii) {
    string_u8_reverse_codepoints(&small);
    ck_assert(string_equals(&small, "olleh"));
}
END_TEST

START_TEST(string_u8_reverse_codepoints_utf8) {
    String kana = string_create(KANA);
    string_u8_reverse_codepoints(&kana);

    ck_assert(string_equals(&kana, "わちにんこ"));
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_join_none) {
    ck_assert(string_join(NULL, NULL, NULL, 0));
}
END_TEST

START_TEST(string_join_two) {
    String segments[2];
    String comma = string_create(", ");
    String result = string_create("");
    ck_assert(string_init(&segments[0], "Hello"));
    ck_assert(string_init(&segments[1], "my name is ..."));
    ck_assert(string_join(&result, &comma, segments, 2));
    ck_assert(string_equals(&result, "Hello, my name is ..."));
    string_free_resources(&segments[0]);
    string_free_resources(&segments[1]);
    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_join_many) {
    String segments[5];
    String comma = string_create(", ");
    String result = string_create("List: ");
    ck_assert(string_init(&segments[0], "Milk"));
    ck_assert(string_init(&segments[1], "Apples"));
    ck_assert(string_init(&segments[2], "Bananas"));
    ck_assert(string_init(&segments[3], "Sandwich Meat"));
    ck_assert(string_init(&segments[4], "Bread"));
    ck_assert(string_join(&result, &comma, segments, 5));
    ck_assert(string_equals(&result, "List: Milk, Apples, Bananas, Sandwich Meat, Bread"));

    for(int i = 0; i < 5; i++)
        string_free_resources(&segments[i]);

    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_join_refs_none) {
    ck_assert(string_join_refs(NULL, NULL, NULL, 0));
}
END_TEST

START_TEST(string_join_refs_two) {
    String* segments[2];
    String comma = string_create(", ");
    String result = string_create("");
    ck_assert((segments[0] = string_create_ref("Hello")));
    ck_assert((segments[1] = string_create_ref("my name is ...")));
    ck_assert(string_join_refs(&result, &comma, segments, 2));
    ck_assert(string_equals(&result, "Hello, my name is ..."));
    string_free(&segments[0]);
    string_free(&segments[1]);
    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_join_many) {
    String* segments[5];
    String comma = string_create(", ");
    String result = string_create("List: ");
    ck_assert((&segments[0] = string_create_ref("Milk")));
    ck_assert((&segments[1] = string_create_ref("Apples")));
    ck_assert((&segments[2] = string_create_ref("Bananas")));
    ck_assert((&segments[3] = string_create_ref("Sandwich Meat")));
    ck_assert((&segments[4] = string_create_ref("Bread")));
    ck_assert(string_join(&result, &comma, segments, 5));
    ck_assert(string_equals(&result, "List: Milk, Apples, Bananas, Sandwich Meat, Bread"));

    for(int i = 0; i < 5; i++)
        string_free(&segments[i]);

    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_split_init_into_existing_less_than_size) {
    String results[2];
    
}
END_TEST

START_TEST(string_format_string_new) {
    String format = string_create("%d");
    String* result = string_format_string(NULL, &format, 152);
    ck_assert(result != NULL);
    ck_assert(string_equals(result, "152"));
    string_free(result);
}
END_TEST

START_TEST(string_format_string_existing) {
    String format = string_create("%u");
    String result = string_create("");
    ck_assert(string_format_string(&result, &format, UINT_MAX) != NULL);
    ck_assert(string_equals(&result, "4294967295"));
    string_free_resources(&result);
}
END_TEST

START_TEST(string_format_string_append) {
    String format = string_create("%d");
    String result = string_create("My age is ");
    ck_assert(string_format_string(&result, &format, 23) != NULL);
    ck_assert(string_equals(&result, "My age is 23"));
    string_free_resources(&result);
}
END_TEST

START_TEST(string_format_cstr_new) {
    String* result = string_format_cstr(NULL, "%d", 152);
    ck_assert(result != NULL);
    ck_assert(string_equals(result, "152"));
    string_free(result);
}
END_TEST

START_TEST(string_format_cstr_existing) {
    String result = string_create("");
    ck_assert(string_format_cstr(&result, "%u", UINT_MAX) != NULL);
    ck_assert(string_equals(&result, "4294967295"));
    string_free_resources(&result);
}
END_TEST

START_TEST(string_format_cstr_append) {
    String result = string_create("My age is ");
    ck_assert(string_format_cstr(&result, "%d", 23) != NULL);
    ck_assert(string_equals(&result, "My age is 23"));
    string_free_resources(&result);
}
END_TEST

int main(void) {
    int number_failed;

    Suite* s = suite_create("String");
    TCase* tc = tcase_create("String");

    tcase_add_checked_fixture(tc, string_start, string_reset);

    tcase_add_test(tc, string_small_has_small_flag);
    tcase_add_test(tc, string_long_has_long_flag);
    tcase_add_test(tc, string_switches_size);
    tcase_add_test(tc, string_is_correct_size);
    tcase_add_test(tc, string_init_copies_cstr);
    tcase_add_test(tc, string_small_cstr);
    tcase_add_test(tc, string_large_cstr);
    tcase_add_test(tc, string_small_size);
    tcase_add_test(tc, string_large_size);
    tcase_add_test(tc, string_size_dynamic);
    tcase_add_test(tc, string_size_utf8);
    tcase_add_test(tc, string_u8_codepoints_ascii);
    tcase_add_test(tc, string_u8_codepoints_utf8);
    tcase_add_test(tc, string_u8_codepoints_all);
    tcase_add_test(tc, string_get_all);
    tcase_add_test(tc, string_u8_get_ascii);
    tcase_add_test(tc, string_u8_get_utf8);
    tcase_add_test(tc, string_u8_get_all);
    tcase_add_test(tc, string_u8_get_with_size_ascii);
    tcase_add_test(tc, string_u8_get_with_size_utf8);
    tcase_add_test(tc, string_u8_get_with_size_all);
    tcase_add_test(tc, string_u8_codepoint_size_ascii);
    tcase_add_test(tc, string_u8_codepoint_size_utf8);
    tcase_add_test(tc, string_u8_codepoint_size_all);
    tcase_add_test(tc, string_set_ascii);
    tcase_add_test(tc, string_set_mixed);
    tcase_add_test(tc, string_u8_set_ascii);
    tcase_add_test(tc, string_u8_set_smaller);
    tcase_add_test(tc, string_u8_set_bigger);
    tcase_add_test(tc, string_empty_empty);
    tcase_add_test(tc, string_empty_whitespace);
    tcase_add_test(tc, string_empty_letters);
    tcase_add_test(tc, string_reserve_small_less_than_capacity);
    tcase_add_test(tc, string_reserve_small_greater_than_capacity);
    tcase_add_test(tc, string_reserve_large_less_than_capacity);
    tcase_add_test(tc, string_reserve_large_greater_than_capacity);
    tcase_add_test(tc, string_shrink_small_to_small);
    tcase_add_test(tc, string_shrink_large_to_large);
    tcase_add_test(tc, string_shrink_large_to_small);
    tcase_add_test(tc, string_clear_small);
    tcase_add_test(tc, string_clear_large);
    tcase_add_test(tc, string_insert_cstr_ascii);
    tcase_add_test(tc, string_insert_cstr_utf8);
    tcase_add_test(tc, string_insert_string_ascii);
    tcase_add_test(tc, string_insert_string_utf8);
    tcase_add_test(tc, string_insert_cstr_part_ascii);
    tcase_add_test(tc, string_insert_cstr_part_utf8);
    tcase_add_test(tc, string_insert_string_part_ascii);
    tcase_add_test(tc, string_insert_string_part_utf8);
    tcase_add_test(tc, string_erase_small_start);
    tcase_add_test(tc, string_erase_small_end);
    tcase_add_test(tc, string_erase_small_middle);
    tcase_add_test(tc, string_erase_large_start);
    tcase_add_test(tc, string_erase_large_end);
    tcase_add_test(tc, string_erase_large_middle);
    tcase_add_test(tc, string_push_back_succeeds);
    tcase_add_test(tc, string_u8_push_back_ascii);
    tcase_add_test(tc, string_u8_push_back_utf8);
    tcase_add_test(tc, string_u8_push_back_all);
    tcase_add_test(tc, string_pop_back_succeeds);
    tcase_add_test(tc, string_u8_pop_back_ascii);
    tcase_add_test(tc, string_u8_pop_back_utf8);
    tcase_add_test(tc, string_u8_pop_back_all);
    tcase_add_test(tc, string_append_cstr_small_to_small);
    tcase_add_test(tc, string_append_cstr_large_to_large);
    tcase_add_test(tc, string_append_cstr_small_to_large);
    tcase_add_test(tc, string_append_string_small_to_small);
    tcase_add_test(tc, string_append_string_large_to_large);
    tcase_add_test(tc, string_append_string_small_to_large);
    tcase_add_test(tc, string_append_cstr_part_succeeds);
    tcase_add_test(tc, string_append_string_part_succeeds);
    tcase_add_test(tc, string_compare_cstr_less_than);
    tcase_add_test(tc, string_compare_cstr_greater_than);
    tcase_add_test(tc, string_compare_cstr_equals);
    tcase_add_test(tc, string_compare_string_less_than);
    tcase_add_test(tc, string_compare_string_greater_than);
    tcase_add_test(tc, string_compare_string_equals);
    tcase_add_test(tc, string_equals_cstr_true);
    tcase_add_test(tc, string_equals_cstr_false);
    tcase_add_test(tc, string_equals_string_true);
    tcase_add_test(tc, string_equals_string_false);
    tcase_add_test(tc, string_starts_with_cstr_true);
    tcase_add_test(tc, string_starts_with_cstr_false);
    tcase_add_test(tc, string_starts_with_string_true);
    tcase_add_test(tc, string_starts_with_string_false);
    tcase_add_test(tc, string_ends_with_cstr_true);
    tcase_add_test(tc, string_ends_with_cstr_false);
    tcase_add_test(tc, string_ends_with_string_true);
    tcase_add_test(tc, string_ends_with_string_false);
    tcase_add_test(tc, string_replace_cstr_shrink);
    tcase_add_test(tc, string_replace_cstr_shrink_end);
    tcase_add_test(tc, string_replace_cstr_same_size);
    tcase_add_test(tc, string_replace_cstr_grow_small_to_small);
    tcase_add_test(tc, string_replace_cstr_grow_small_to_large);
    tcase_add_test(tc, string_replace_string_shrink);
    tcase_add_test(tc, string_replace_string_shrink_end);
    tcase_add_test(tc, string_replace_string_same_size);
    tcase_add_test(tc, string_replace_string_grow_small_to_small);
    tcase_add_test(tc, string_replace_string_grow_small_to_large);
    tcase_add_test(tc, string_substring_part);
    tcase_add_test(tc, string_substring_whole);
    tcase_add_test(tc, string_copy_small);
    tcase_add_test(tc, string_copy_large);
    tcase_add_test(tc, string_copy_to_short);
    tcase_add_test(tc, string_copy_to_large);
    tcase_add_test(tc, string_resize_shrink);
    tcase_add_test(tc, string_resize_same_size);
    tcase_add_test(tc, string_resize_grow);
    tcase_add_test(tc, string_swap_small_and_large);
    tcase_add_test(tc, string_find_cstr_first);
    tcase_add_test(tc, string_find_cstr_second);
    tcase_add_test(tc, string_find_cstr_none);
    tcase_add_test(tc, string_find_string_first);
    tcase_add_test(tc, string_find_string_second);
    tcase_add_test(tc, string_find_string_none);
    tcase_add_test(tc, string_rfind_cstr_first);
    tcase_add_test(tc, string_rfind_cstr_second);
    tcase_add_test(tc, string_rfind_cstr_none);
    tcase_add_test(tc, string_rfind_string_first);
    tcase_add_test(tc, string_rfind_string_second);
    tcase_add_test(tc, string_rfind_string_none);
    tcase_add_test(tc, string_is_null_or_empty_null);
    tcase_add_test(tc, string_is_null_or_empty_empty);
    tcase_add_test(tc, string_is_null_or_empty_filled);
    tcase_add_test(tc, string_is_null_or_whitespace_empty);
    tcase_add_test(tc, string_is_null_or_whitespace_space);
    tcase_add_test(tc, string_is_null_or_whitespace_u8_space);
    tcase_add_test(tc, string_is_null_or_whitespace_filled);
    tcase_add_test(tc, string_reverse_bytes_ascii);
    tcase_add_test(tc, string_reverse_bytes_utf8);
    tcase_add_test(tc, string_u8_reverse_codepoints_ascii);
    tcase_add_test(tc, string_u8_reverse_codepoints_utf8);
    tcase_add_test(tc, string_join_none);
    tcase_add_test(tc, string_join_two);
    tcase_add_test(tc, string_join_many);
    tcase_add_test(tc, string_format_string_new);
    tcase_add_test(tc, string_format_string_existing);
    tcase_add_test(tc, string_format_string_append);
    tcase_add_test(tc, string_format_cstr_new);
    tcase_add_test(tc, string_format_cstr_existing);
    tcase_add_test(tc, string_format_cstr_append);


    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}