#include <check.h>

#include "./test_macros.h"
#include "../include/sso_string.h"

static String small;
static String large;

#define HELLO "hello"
#define HELLO_SIZE 5
#define KANA "こんにちは"
#define KANA_SIZE 15
#define KANA_WIDE L"\u3053\u3093\u306B\u3061\u306F"
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_SIZE 26
#define MIXED "hこeんlいlちoは"
#define MIXED_SIZE 20
#define ALL_CODEPOINTS { 0x0001, 0x007F, 0x00080, 0x007FF, 0x00800, 0x0FFFF, 0x10000, 0x10FFFF }
#define ALL_CODEPOINTS_SIZE 20

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

bool test_malloc_fail = false;
bool test_assert_arg_fail = false;
bool test_assert_bounds_fail = false;

static void string_start(void) {
    string_init(&small, "hello");
    string_init(&large, ALPHABET);
}

static void string_reset(void) {
    string_free_resources(&small);
    string_free_resources(&large);
    test_malloc_fail = false;
    test_assert_arg_fail = false;
    test_assert_bounds_fail = false;
}

START_TEST(string_small_has_small_flag) {
    String str = STRING_EMPTY;
    ck_assert(!sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_long_has_long_flag) {
    String str = string_create(ALPHABET);
    ck_assert(sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_switches_size) {
    // This test not only checks that the string
    // switches size correctly, but also that
    // the capacity behaviour is the same for small strings
    // and large strings.

    String str = STRING_EMPTY;

    int cap = string_capacity(&str);

    while(string_size(&str) <= cap) {
        ck_assert(!sso_string_is_long(&str));
        string_push(&str, 'a');
    }

    ck_assert(sso_string_is_long(&str));

    cap = string_capacity(&str);

    while(string_size(&str) <= cap) {
        ck_assert(string_capacity(&str) == cap);
        string_push(&str, 'b');
    }

    ck_assert(string_capacity(&str) != cap);

    cap = string_capacity(&str);

    while(string_size(&str) <= cap) {
        ck_assert(string_capacity(&str) == cap);
        string_push(&str, 'c');
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
    String str = STRING_EMPTY;
    for(int i = 0; i < 100; i++) {
        ck_assert(string_size(&str) == i);
        string_push(&str, 'a');
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
        string_u8_push(&str, all_codepoints[i]);

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
    String str = STRING_EMPTY;
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        string_u8_push(&str, all_codepoints[i]);

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
    String str = STRING_EMPTY;
    
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push(&str, all_codepoints[i]));

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
    String str = STRING_EMPTY;
    
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push(&str, all_codepoints[i]));

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
    String str = STRING_EMPTY;
    
    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push(&str, all_codepoints[i]));

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
    char expected[] = "こnにちは";

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
    String str = STRING_EMPTY;
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
        string_push(&large, 'a');

    ck_assert(string_size(&large) != string_capacity(&large));
    string_shrink_to_fit(&large);
    ck_assert(string_size(&large) == string_capacity(&large));
}
END_TEST

START_TEST(string_shrink_large_to_small) {
    string_clear(&large);
    ck_assert(sso_string_is_long(&large));
    string_shrink_to_fit(&large);
    ck_assert(!sso_string_is_long(&large));

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

START_TEST(string_insert_part_cstr_ascii) {
    String str = string_create("lexd");
    char* result = "alexander";

    ck_assert(string_insert_part_cstr(&str, result, 0, 0, 1));
    ck_assert(strcmp(string_data(&str), "alexd") == 0);

    ck_assert(string_insert_part_cstr(&str, result, 4, 4, 2));
    ck_assert(strcmp(string_data(&str), "alexand") == 0);

    ck_assert(string_insert_part_cstr(&str, result, 7, 7, 2));
    ck_assert(strcmp(string_data(&str), result) == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_part_cstr_utf8) {
    String str = string_create("んち");
    char* result = "こんにちは。";

    ck_assert(string_insert_part_cstr(&str, result, 0, 0, 3));
    ck_assert(strcmp(string_data(&str), "こんち") == 0);

    ck_assert(string_insert_part_cstr(&str, result, 6, 6, 3));
    ck_assert(strcmp(string_data(&str), "こんにち") == 0);

    ck_assert(string_insert_part_cstr(&str, result, 12, 12, 6));
    ck_assert(strcmp(string_data(&str), "こんにちは。") == 0);

    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_part_string_ascii) {
    String str = string_create("lexd");
    String result = string_create("alexander");

    ck_assert(string_insert_part_string(&str, &result, 0, 0, 1));
    ck_assert(strcmp(string_data(&str), "alexd") == 0);

    ck_assert(string_insert_part_string(&str, &result, 4, 4, 2));
    ck_assert(strcmp(string_data(&str), "alexand") == 0);

    ck_assert(string_insert_part_string(&str, &result, 7, 7, 2));
    ck_assert(strcmp(string_data(&str), "alexander") == 0);

    string_free_resources(&str);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_insert_part_string_utf8) {
    String str = string_create("んち");
    String result = string_create("こんにちは。");

    ck_assert(string_insert_part_string(&str, &result, 0, 0, 3));
    ck_assert(strcmp(string_data(&str), "こんち") == 0);

    ck_assert(string_insert_part_string(&str, &result, 6, 6, 3));
    ck_assert(strcmp(string_data(&str), "こんにち") == 0);

    ck_assert(string_insert_part_string(&str, &result, 12, 12, 6));
    ck_assert(strcmp(string_data(&str), "こんにちは。") == 0);

    string_free_resources(&str);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_insert_self_before_insert_point) {
    String str = string_create("helloworld");
    ck_assert(string_insert_part_string(&str, &str, 6, 1, 4));
    ck_assert(string_equals_cstr(&str, "hellowelloorld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_self_after_insert_point) {
    String str = string_create("helloworld");
    ck_assert(string_insert_part_string(&str, &str, 0, 5, 5));
    ck_assert(string_equals_cstr(&str, "worldhelloworld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_self_overlapping_insert_point) {
    String str = string_create("helloworld");
    ck_assert(string_insert_part_string(&str, &str, 7, 5, 5));
    ck_assert(string_equals_cstr(&str, "hellowoworldrld"));
    string_free_resources(&str);
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

START_TEST(string_push_succeeds) {
    String str = STRING_EMPTY;
    ck_assert(string_push(&str, 'h'));
    ck_assert(string_equals_cstr(&str, "h"));

    ck_assert(string_push(&str, 'e'));
    ck_assert(string_equals_cstr(&str, "he"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_push_ascii) {
    String str = STRING_EMPTY;
    ck_assert(string_u8_push(&str, 'h'));
    ck_assert(string_equals_cstr(&str, "h"));

    ck_assert(string_u8_push(&str, 'e'));
    ck_assert(string_equals_cstr(&str, "he"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_push_utf8) {
    String str = STRING_EMPTY;
    wchar_t kana[] = KANA_WIDE;
    string_u8_push(&str, kana[0]);
    ck_assert(string_equals_cstr(&str, "こ"));

    string_u8_push(&str, kana[1]);
    ck_assert(string_equals_cstr(&str, "こん"));

    string_free_resources(&str);
}
END_TEST

START_TEST(string_u8_push_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = STRING_EMPTY;

    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push(&str, all_codepoints[i]));

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

START_TEST(string_pop_succeeds) {
    char* hello = HELLO;
    for(int i = HELLO_SIZE - 1; i >= 0; i--) {
        char letter = string_pop(&small);
        ck_assert(letter = hello[i]);
    }
}
END_TEST

START_TEST(string_u8_pop_ascii) {
    char* hello = HELLO;
    for(int i = HELLO_SIZE - 1; i >= 0; i--) {
        Char32 letter = string_u8_pop(&small);
        ck_assert(letter = hello[i]);
    }
}
END_TEST

START_TEST(string_u8_pop_utf8) {
    String str = string_create(KANA);
    String result = string_create(KANA);
    int codepoints = string_u8_codepoints(&result);
    int size = string_size(&str);

    for(int i = codepoints - 1; i >= 0; i--) {
        Char32 kana = string_u8_pop(&str);
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

START_TEST(string_u8_pop_all) {
    Char32 all_codepoints[] = ALL_CODEPOINTS;
    String str = STRING_EMPTY;

    for(int i = 0; i < ARRAY_SIZE(all_codepoints); i++)
        ck_assert(string_u8_push(&str, all_codepoints[i]));

    String result = string_create(string_data(&str));

    int expected_size = 4;
    int offset = 4;
    size_t size = string_size(&result);

    for(int i = ARRAY_SIZE(all_codepoints) - 1; i >= 0; i--) {
        Char32 letter = string_u8_pop(&str);
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
    String str = STRING_EMPTY;
    ck_assert(!sso_string_is_long(&str));
    ck_assert(string_append_cstr(&str, ALPHABET));
    ck_assert(string_equals_cstr(&str, ALPHABET));
    ck_assert(sso_string_is_long(&str));

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
    String str = STRING_EMPTY;
    String value = string_create(ALPHABET);
    ck_assert(!sso_string_is_long(&str));
    ck_assert(string_append_string(&str, &value));
    ck_assert(string_equals_cstr(&str, ALPHABET));
    ck_assert(sso_string_is_long(&str));

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

START_TEST(string_append_self_whole) {
    ck_assert(string_append_string(&small, &small));
    ck_assert(string_equals_cstr(&small, HELLO HELLO));
}
END_TEST

START_TEST(string_append_self_part) {
    String str = string_create("helloworld");
    ck_assert(string_append_string_part(&str, &str, 0, 5));
    ck_assert(string_equals_cstr(&str, "helloworldhello"));
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

START_TEST(string_trim_string_both) {
    String value = string_create("abba");
    String a = string_create("a");
    string_trim_string(&value, &a);
    ck_assert(string_equals_cstr(&value, "bb"));
    string_free_resources(&value);
    string_free_resources(&a);
}
END_TEST

START_TEST(string_trim_string_left) {
    String h = string_create("h");
    string_trim_string(&small, &h);
    ck_assert(string_equals_cstr(&small, "ello"));
    string_free_resources(&h);
}
END_TEST

START_TEST(string_trim_string_right) {
    String o = string_create("o");
    string_trim_string(&small, &o);
    ck_assert(string_equals_cstr(&small, "hell"));
    string_free_resources(&o);
}
END_TEST

START_TEST(string_trim_string_neither) {
    String b = string_create("b");
    string_trim_string(&small, &b);
    ck_assert(string_equals_cstr(&small, HELLO));
    string_free_resources(&b);
}
END_TEST

START_TEST(string_trim_string_multiple_letters) {
    String value = string_create("aabbaa");
    String aa = string_create("aa");
    string_trim_string(&value, &aa);
    ck_assert(string_equals_cstr(&value, "bb"));
    string_free_resources(&value);
    string_free_resources(&aa);
}
END_TEST

START_TEST(string_trim_string_multiple_sequences) {
    String value = string_create("aabcaa");
    String a = string_create("a");
    string_trim_string(&value, &a);
    ck_assert(string_equals_cstr(&value, "bc"));
    string_free_resources(&value);
    string_free_resources(&a);
}
END_TEST

START_TEST(string_trim_string_full_sequence_only) {
    String value = string_create("aaabbaaa");
    String aa = string_create("aa");
    string_trim_string(&value, &aa);
    ck_assert(string_equals_cstr(&value, "abba"));
    string_free_resources(&value);
    string_free_resources(&aa);
}
END_TEST

START_TEST(string_trim_any_string_single) {
    String value = string_create("abcba");
    String values[2] = { string_create("a"), string_create("z") };
    string_trim_any_string(&value, values, 2);
    ck_assert(string_equals_cstr(&value, "bcb"));
    string_free_resources(&value);
    for(int i = 0; i < ARRAY_SIZE(values); i++) {
        string_free_resources(values + i);
    }
}
END_TEST

START_TEST(string_trim_any_string_multiple) {
    String value = string_create("abcab");
    String values[2] = { string_create("a"), string_create("b") };
    string_trim_any_string(&value, values, 2);
    ck_assert(string_equals_cstr(&value, "c"));
    string_free_resources(&value);
    for(int i = 0; i < ARRAY_SIZE(values); i++) {
        string_free_resources(values + i);
    }
}
END_TEST

START_TEST(string_pad_left_none) {
    ck_assert(string_pad_left(&small, ' ', 5));
    ck_assert(string_size(&small) == 5);
    ck_assert(string_equals_cstr(&small, HELLO));
}
END_TEST

START_TEST(string_pad_left_some) {
    ck_assert(string_pad_left(&small, ' ', 7));
    ck_assert(string_size(&small) == 7);
    ck_assert(string_equals_cstr(&small, "  " HELLO));
}
END_TEST

START_TEST(string_pad_right_none) {
    ck_assert(string_pad_right(&small, ' ', 5));
    ck_assert(string_size(&small) == 5);
    ck_assert(string_equals_cstr(&small, HELLO));
}
END_TEST

START_TEST(string_pad_right_some) {
    ck_assert(string_pad_right(&small, ' ', 7));
    ck_assert(string_size(&small) == 7);
    ck_assert(string_equals_cstr(&small, HELLO "  "));
}
END_TEST

START_TEST(string_u8_pad_left_none) {
    String kana = string_create(KANA);
    ck_assert(string_u8_pad_left(&kana, ' ', 5));
    ck_assert(string_u8_codepoints(&kana) == 5);
    ck_assert(string_equals_cstr(&kana, KANA));
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_u8_pad_left_string_ascii_value_ascii) {
    ck_assert(string_u8_pad_left(&small, ' ', 7));
    ck_assert(string_u8_codepoints(&small) == 7);
    ck_assert(string_equals_cstr(&small, "  " HELLO));
}
END_TEST

START_TEST(string_u8_pad_left_string_u8_value_ascii) {
    String kana = string_create(KANA);
    ck_assert(string_u8_pad_left(&kana, ' ', 7));
    ck_assert(string_u8_codepoints(&kana) == 7);
    ck_assert(string_equals_cstr(&kana, "  " KANA));
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_u8_pad_left_string_u8_value_u8) {
    String kana = string_create(KANA);
    ck_assert(string_u8_pad_left(&kana, '。', 7));
    ck_assert(string_u8_codepoints(&kana) == 7);
    ck_assert(string_equals_cstr(&kana, "。。" KANA));
}
END_TEST

START_TEST(string_u8_pad_right_none) {
    String kana = string_create(KANA);
    ck_assert(string_u8_pad_right(&kana, ' ', 5));
    ck_assert(string_u8_codepoints(&kana) == 5);
    ck_assert(string_equals_cstr(&kana, KANA));
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_u8_pad_right_string_ascii_value_ascii) {
    ck_assert(string_u8_pad_right(&small, ' ', 7));
    ck_assert(string_u8_codepoints(&small) == 7);
    ck_assert(string_equals_cstr(&small, HELLO "  "));
}
END_TEST

START_TEST(string_u8_pad_right_string_u8_value_ascii) {
    String kana = string_create(KANA);
    ck_assert(string_u8_pad_right(&kana, ' ', 7));
    ck_assert(string_u8_codepoints(&kana) == 7);
    ck_assert(string_equals_cstr(&kana, KANA "  "));
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_u8_pad_right_string_u8_value_u8) {
    String kana = string_create(KANA);
    ck_assert(string_u8_pad_right(&kana, '。', 7));
    ck_assert(string_u8_codepoints(&kana) == 7);
    ck_assert(string_equals_cstr(&kana, KANA "。。"));
    string_free_resources(&kana);
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

START_TEST(string_replace_self_large_to_small_substring_before_replace) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 6, 3, &str, 2, 2));
    ck_assert(string_equals_cstr(&str, "hellowlld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_self_large_to_small_substring_after_replace) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 1, 3, &str, 4, 2));
    ck_assert(string_equals_cstr(&str, "howoworld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_self_large_to_small_overlapping) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 1, 4, &str, 2, 2));
    ck_assert(string_equals_cstr(&str, "hllworld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_self_small_to_large_before_replace) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 6, 2, &str, 0, 3));
    ck_assert(string_equals_cstr(&str, "hellowhelld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_self_small_to_large_after_replace) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 1, 2, &str, 5, 3));
    ck_assert(string_equals_cstr(&str, "hworloworld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_self_small_to_large_overlapping_right) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 3, 2, &str, 4, 3));
    ck_assert(string_equals_cstr(&str, "helowoworld"));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_self_small_to_large_overlapping_left) {
    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 5, 2, &str, 4, 3));
    ck_assert(string_equals_cstr(&str, "hellooworld"));
    string_free_resources(&str);
}
START_TEST(string_replace_self_small_to_large_overlapping_whole) {
END_TEST

    String str = string_create("helloworld");
    ck_assert(string_replace_part_string(&str, 2, 2, &str, 0, 3));
    ck_assert(string_equals_cstr(&str, "heheloworld"));
    string_free_resources(&str);
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
    ck_assert(string_rfind_cstr(&str, 0, HELLO) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_cstr_second) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_cstr(&str, 6, HELLO) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_cstr_none) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_cstr(&str, 0, "moo") == SIZE_MAX);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_string_first) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_string(&str, 0, &small) == 5);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_string_second) {
    String str = string_create(HELLO HELLO);
    ck_assert(string_rfind_string(&str, 6, &small) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_rfind_string_none) {
    String str = string_create(HELLO HELLO);
    String moo = string_create("moo");
    ck_assert(string_rfind_string(&str, 0, &moo) == SIZE_MAX);
    string_free_resources(&str);
    string_free_resources(&moo);
}
END_TEST

START_TEST(string_is_null_or_empty_null) {
    ck_assert(string_is_null_or_empty(NULL));
}
END_TEST

START_TEST(string_is_null_or_empty_empty) {
    String str = STRING_EMPTY;
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
    String str = STRING_EMPTY;
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
    String str = STRING_EMPTY;
    string_u8_push(&str, ' '); // ascii space
    string_u8_push(&str, L'\xA0'); // no-break space
    string_u8_push(&str, L'\u2009'); // thin space
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

    ck_assert(string_equals(&kana, "はちにんこ"));
    string_free_resources(&kana);
}
END_TEST

START_TEST(string_join_string_none) {
    String empty = STRING_EMPTY;
    ck_assert(string_join_string(NULL, &empty, NULL, 0));
}
END_TEST

START_TEST(string_join_string_two) {
    String segments[2];
    String comma = string_create(", ");
    String result = STRING_EMPTY;
    ck_assert(string_init(&segments[0], "Hello"));
    ck_assert(string_init(&segments[1], "my name is ..."));
    ck_assert(string_join_string(&result, &comma, segments, 2));
    ck_assert(string_equals(&result, "Hello, my name is ..."));
    string_free_resources(&segments[0]);
    string_free_resources(&segments[1]);
    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_join_string_many) {
    String segments[5];
    String comma = string_create(", ");
    String result = string_create("List: ");
    ck_assert(string_init(&segments[0], "Milk"));
    ck_assert(string_init(&segments[1], "Apples"));
    ck_assert(string_init(&segments[2], "Bananas"));
    ck_assert(string_init(&segments[3], "Sandwich Meat"));
    ck_assert(string_init(&segments[4], "Bread"));
    ck_assert(string_join_string(&result, &comma, segments, 5));
    ck_assert(string_equals(&result, "List: Milk, Apples, Bananas, Sandwich Meat, Bread"));

    for(int i = 0; i < 5; i++)
        string_free_resources(&segments[i]);

    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_join_refs_string_none) {
    String empty = STRING_EMPTY;
    ck_assert(string_join_refs_string(NULL, &empty, NULL, 0));
}
END_TEST

START_TEST(string_join_refs_string_two) {
    String* segments[2];
    String comma = string_create(", ");
    String result = STRING_EMPTY;
    ck_assert((segments[0] = string_create_ref("Hello")));
    ck_assert((segments[1] = string_create_ref("my name is ...")));
    ck_assert(string_join_refs_string(&result, &comma, segments, 2));
    ck_assert(string_equals(&result, "Hello, my name is ..."));
    string_free(segments[0]);
    string_free(segments[1]);
    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_join_refs_string_many) {
    String* segments[5];
    String comma = string_create(", ");
    String result = string_create("List: ");
    ck_assert((segments[0] = string_create_ref("Milk")));
    ck_assert((segments[1] = string_create_ref("Apples")));
    ck_assert((segments[2] = string_create_ref("Bananas")));
    ck_assert((segments[3] = string_create_ref("Sandwich Meat")));
    ck_assert((segments[4] = string_create_ref("Bread")));
    ck_assert(string_join_refs_string(&result, &comma, segments, 5));
    ck_assert(string_equals(&result, "List: Milk, Apples, Bananas, Sandwich Meat, Bread"));

    for(int i = 0; i < 5; i++)
        string_free(segments[i]);

    string_free_resources(&comma);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_split_string_init_into_existing_less_than_size) {
    String results[4];
    String to_split = string_create("moo, caw, meow");
    String separator = string_create(", ");
    int filled;
    String* result = string_split_string(&to_split, &separator, results, 4, &filled, false, true);
    ck_assert(result);
    ck_assert(result == results);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(results + 0, "moo"));
    ck_assert(string_equals_cstr(results + 1, "caw"));
    ck_assert(string_equals_cstr(results + 2, "meow"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(results + i);
    }

    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_init_into_existing_equal_size) {
    String results[3];
    String to_split = string_create("Who am I");
    String separator = string_create(" ");
    int filled;
    String* result = string_split_string(&to_split, &separator, results, 3, &filled, false, true);
    ck_assert(result);
    ck_assert(result == results);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(results + 0, "Who"));
    ck_assert(string_equals_cstr(results + 1, "am"));
    ck_assert(string_equals_cstr(results + 2, "I"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(results + i);
    }

    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_init_into_existing_greater_than_size) {
    String results[2];
    String to_split = string_create("What is your name my friend");
    String separator = string_create(" ");
    int filled;
    String* result = string_split_string(&to_split, &separator, results, 2, &filled, false, true);
    ck_assert(result);
    ck_assert(result == results);
    ck_assert(filled == 2);
    ck_assert(string_equals_cstr(results + 0, "What"));
    ck_assert(string_equals_cstr(results + 1, "is"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(results + i);
    }

    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_no_init_into_existing_less_than_size) {
    String results[4];
    for(int i = 0; i < 4; i++) {
        ck_assert(string_init(results + i, "init"));
    }
    String to_split = string_create("moo, caw, meow");
    String separator = string_create(", ");
    int filled;
    String* result = string_split_string(&to_split, &separator, results, 4, &filled, false, false);
    ck_assert(result);
    ck_assert(result == results);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(results + 0, "initmoo"));
    ck_assert(string_equals_cstr(results + 1, "initcaw"));
    ck_assert(string_equals_cstr(results + 2, "initmeow"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(results + i);
    }

    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_no_init_into_existing_equal_size) {
    String results[3];
    for(int i = 0; i < 3; i++) {
        ck_assert(string_init(results + i, "init"));
    }
    String to_split = string_create("Who am I");
    String separator = string_create(" ");
    int filled;
    String* result = string_split_string(&to_split, &separator, results, 3, &filled, false, false);
    ck_assert(result);
    ck_assert(result == results);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(results + 0, "initWho"));
    ck_assert(string_equals_cstr(results + 1, "initam"));
    ck_assert(string_equals_cstr(results + 2, "initI"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(results + i);
    }

    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_no_init_into_existing_greater_than_size) {
    String results[2];
    for(int i = 0; i < 2; i++) {
        ck_assert(string_init(results + i, "init"));
    }
    String to_split = string_create("What is your name my friend");
    String separator = string_create(" ");
    int filled;
    String* result = string_split_string(&to_split, &separator, results, 2, &filled, false, false);
    ck_assert(result);
    ck_assert(result == results);
    ck_assert(filled == 2);
    ck_assert(string_equals_cstr(results + 0, "initWhat"));
    ck_assert(string_equals_cstr(results + 1, "initis"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(results + i);
    }

    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_allocate_init) {
    String to_split = string_create("moo, caw, meow");
    String separator = string_create(", ");
    int filled;
    String* result = string_split_string(&to_split, &separator, NULL, STRING_SPLIT_ALLOCATE, &filled, false, true);
    ck_assert(result);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(result + 0, "moo"));
    ck_assert(string_equals_cstr(result + 1, "caw"));
    ck_assert(string_equals_cstr(result + 2, "meow"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(result + i);
    }

    free(result);
    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_allocate_no_init) {
    // This should work exactly the same as string_split_string_allocate_init, since
    // it should be set to init no matter what if results_count < 0.

    String to_split = string_create("moo, caw, meow");
    String separator = string_create(", ");
    int filled;
    String* result = string_split_string(&to_split, &separator, NULL, STRING_SPLIT_ALLOCATE, &filled, false, false);
    ck_assert(result);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(result + 0, "moo"));
    ck_assert(string_equals_cstr(result + 1, "caw"));
    ck_assert(string_equals_cstr(result + 2, "meow"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(result + i);
    }

    free(result);
    string_free_resources(&to_split);
}
END_TEST

START_TEST(string_split_string_skip_empty) {
    String to_split = string_create("double  space");
    String separator = string_create(" ");
    int filled;
    String* result = string_split_string(&to_split, &separator, NULL, STRING_SPLIT_ALLOCATE, &filled, true, true);

    ck_assert(result);
    ck_assert(filled == 2);
    ck_assert(string_equals_cstr(result + 0, "double"));
    ck_assert(string_equals_cstr(result + 1, "space"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(result + i);
    }

    free(result);
    string_free_resources(&to_split);

}
END_TEST

START_TEST(string_split_string_dont_skip_empty) {
    String to_split = string_create("double  space");
    String separator = string_create(" ");
    int filled;
    String* result = string_split_string(&to_split, &separator, NULL, STRING_SPLIT_ALLOCATE, &filled, false, true);

    ck_assert(result);
    ck_assert(filled == 3);
    ck_assert(string_equals_cstr(result + 0, "double"));
    ck_assert(string_equals_cstr(result + 1, ""));
    ck_assert(string_equals_cstr(result + 2, "space"));

    for(int i = 0; i < filled; i++) {
        string_free_resources(result + i);
    }

    free(result);
    string_free_resources(&to_split);

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
    String result = STRING_EMPTY;
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
    String result = STRING_EMPTY;
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

START_TEST(string_format_time_string_new) {
    time_t rawtime = time(NULL);
    struct tm* current_time;
    current_time = localtime(&rawtime);

    String format = string_create("%c");

    char buffer[512];
    ck_assert(strftime(buffer, 512, string_data(&format), current_time) != 0);

    String* result = string_format_time_string(NULL, &format, current_time);
    ck_assert(result != NULL);
    ck_assert(string_equals_cstr(result, buffer));
    string_free(result);
}
END_TEST

START_TEST(string_format_time_string_existing) {
    time_t rawtime = time(NULL);
    struct tm* current_time;
    current_time = localtime(&rawtime);

    String format = string_create("%c");

    char buffer[512];
    ck_assert(strftime(buffer, 512, string_data(&format), current_time) != 0);

    String result = STRING_EMPTY;

    ck_assert(string_format_time_string(&result, &format, current_time) != NULL);
    ck_assert(string_equals_cstr(&result, buffer));
    string_free_resources(&result);
}
END_TEST

START_TEST(string_format_time_string_append) {
    time_t rawtime = time(NULL);
    struct tm* current_time;
    current_time = localtime(&rawtime);

    String format = string_create("%c");

    char buffer[512];
    ck_assert(strftime(buffer, 512, string_data(&format), current_time) != 0);

    String result = string_create("hello");

    ck_assert(string_format_time_string(&result, &format, current_time) != NULL);
    ck_assert(string_find_cstr(&result, 0, buffer) == 5);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_format_time_cstr_new) {
    time_t rawtime = time(NULL);
    struct tm* current_time;
    current_time = localtime(&rawtime);

    char buffer[512];
    ck_assert(strftime(buffer, 512, "%c", current_time) != 0);

    String* result = string_format_time_cstr(NULL, "%c", current_time);
    ck_assert(result != NULL);
    ck_assert(string_equals_cstr(result, buffer));
    string_free(result);
}
END_TEST

START_TEST(string_format_time_cstr_existing) {
    time_t rawtime = time(NULL);
    struct tm* current_time;
    current_time = localtime(&rawtime);

    char buffer[512];
    ck_assert(strftime(buffer, 512, "%c", current_time) != 0);

    String result = STRING_EMPTY;

    ck_assert(string_format_time_cstr(&result, "%c", current_time) != NULL);
    ck_assert(string_equals_cstr(&result, buffer));
    string_free_resources(&result);
}
END_TEST

START_TEST(string_format_time_cstr_append) {
    time_t rawtime = time(NULL);
    struct tm* current_time;
    current_time = localtime(&rawtime);

    char buffer[512];
    ck_assert(strftime(buffer, 512, "%c", current_time) != 0);

    String result = string_create("hello");

    ck_assert(string_format_time_cstr(&result, "%c", current_time) != NULL);
    ck_assert(string_find_cstr(&result, 0, buffer) == 5);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_hash_verify) {
    String str = string_create("Hello world, it is I, your master.");
    size_t hash_result = string_hash(&str);

#if SSO_STRING_SHIFT == 24
    ck_assert(hash_result == 0x7d8ee846);
#elif SSO_STRING_SHIFT == 56
    ck_assert(hash_result == 0xe288df2017a06dc6);
#endif

    string_free_resources(&str);
}
END_TEST

START_TEST(string_create_ref_fail_oom) {
    test_malloc_fail = true;
    ck_assert(!string_create_ref(ALPHABET));
}
END_TEST 

START_TEST(string_init_fail_null_arg) {
    string_init(NULL, NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_init_fail_oom) {
    test_malloc_fail = true;
    String str;
    ck_assert(!string_init(&str, ALPHABET));
}
END_TEST

START_TEST(string_init_size_fail_null_arg) {
    string_init_size(NULL, "Hello", 6);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_init_size_fail_oob) {
    String str;
    string_init_size(&str, "Hello", 6);
    ck_assert(test_assert_bounds_fail);
}
END_TEST

START_TEST(string_init_size_fail_oom) {
    test_malloc_fail = true;
    String str;
    ck_assert(!string_init_size(&str, ALPHABET, ALPHABET_SIZE));
}
END_TEST

START_TEST(string_u8_codepoints_fail_null_arg) {
    string_u8_codepoints(NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_u8_get_fail_null_arg) {
    string_u8_get(NULL, 0);
    ck_assert(test_assert_arg_fail);
} 
END_TEST

START_TEST(string_u8_get_with_size_fail_null_arg) {
    string_u8_get_with_size(NULL, 9, NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_u8_codepoint_size_fail_null_arg) {
    string_u8_codepoint_size(NULL, 0);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_u8_set_fail_null_arg) {
    string_u8_set(NULL, 0, 'M');
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_u8_set_fail_oom) {
    test_malloc_fail = true;
    String str = string_create("");
    while(string_size(&str) <= string_capacity(&str)) {
        ck_assert(string_push(&str, 'a'));
    }

    ck_assert(!string_u8_set(&str, 0, KANA_WIDE[0]));
}
END_TEST

START_TEST(string_upper_fail_null_arg) {
    string_upper(NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_u8_upper_fail_null_arg) {
    string_u8_upper(NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_lower_fail_null_arg) {
    string_u8_lower(NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_reserve_fail_null_arg) {
    string_reserve(NULL, 1000);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_reserve_fail_short_to_long_oom) {
    test_malloc_fail = true;
    ck_assert(!string_reserve(&small, string_capacity(&small) + 3));
}
END_TEST

START_TEST(string_reserve_fail_long_to_long_oom) {
    test_malloc_fail = true;
    ck_assert(!string_reserve(&large, string_capacity(&large) + 3));
}
END_TEST

START_TEST(string_insert_fail_null_str) {
    string_insert_cstr(NULL, "moo", 0);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_insert_fail_null_value) {
    String str = string_create("");
    string_insert_cstr(&str, NULL, 0);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_insert_fail_oob) {
    String str = string_create("Hello");
    string_insert_cstr(&str, "Hello", 10);
    ck_assert(test_assert_bounds_fail);
}
END_TEST

START_TEST(string_insert_part_fail_value_oob) {
    string_insert_part_cstr(&small, "Hello", 0, 0, 6);
    ck_assert(test_assert_bounds_fail);
    test_assert_bounds_fail = false;
    string_insert_part_cstr(&small, "Hello", 0, 5, 1);
    ck_assert(test_assert_bounds_fail);
}
END_TEST

START_TEST(string_erase_fail_null_str) {
    string_erase(NULL, 0, 0);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_erase_fail_oob) {
    string_erase(&small, 0, 6);
    ck_assert(test_assert_bounds_fail);
    string_erase(&small, 5, 1);
    ck_assert(test_assert_bounds_fail);
}
END_TEST

START_TEST(string_push_fail_null_arg) {
    string_push(NULL, '0');
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_push_fail_short_to_long_oom) {
    test_malloc_fail = true;
    while(string_size(&small) <= string_capacity(&small)) {
        ck_assert(string_push(&small, 'a'));
    }

    ck_assert(!string_push(&small, 'a'));
}
END_TEST

START_TEST(string_push_fail_long_to_long_oom) {
    test_malloc_fail = true;
    while(string_size(&large) <= string_capacity(&large)) {
        ck_assert(string_push(&large, 'a'));
    }

    ck_assert(!string_push(&large, 'a'));
}
END_TEST

START_TEST(string_u8_push_fail_null_arg) {
    string_u8_push(NULL, 'a');
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_pop_fail_null_arg) {
    string_pop(NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_u8_pop_fail_null_arg) {
    string_u8_pop(NULL);
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_append_fail_null_str) {
    string_append_cstr(NULL, "hello");
    ck_assert(test_assert_arg_fail);
}
END_TEST

START_TEST(string_append_fail_null_value) {
    string_append_cstr(&small, NULL);
    ck_assert(test_assert_arg_fail);
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
    tcase_add_test(tc, string_insert_part_cstr_ascii);
    tcase_add_test(tc, string_insert_part_cstr_utf8);
    tcase_add_test(tc, string_insert_part_string_ascii);
    tcase_add_test(tc, string_insert_part_string_utf8);
    tcase_add_test(tc, string_insert_self_before_insert_point);
    tcase_add_test(tc, string_insert_self_after_insert_point);
    tcase_add_test(tc, string_insert_self_overlapping_insert_point);
    tcase_add_test(tc, string_erase_small_start);
    tcase_add_test(tc, string_erase_small_end);
    tcase_add_test(tc, string_erase_small_middle);
    tcase_add_test(tc, string_erase_large_start);
    tcase_add_test(tc, string_erase_large_end);
    tcase_add_test(tc, string_erase_large_middle);
    tcase_add_test(tc, string_push_succeeds);
    tcase_add_test(tc, string_u8_push_ascii);
    tcase_add_test(tc, string_u8_push_utf8);
    tcase_add_test(tc, string_u8_push_all);
    tcase_add_test(tc, string_pop_succeeds);
    tcase_add_test(tc, string_u8_pop_ascii);
    tcase_add_test(tc, string_u8_pop_utf8);
    tcase_add_test(tc, string_u8_pop_all);
    tcase_add_test(tc, string_append_cstr_small_to_small);
    tcase_add_test(tc, string_append_cstr_large_to_large);
    tcase_add_test(tc, string_append_cstr_small_to_large);
    tcase_add_test(tc, string_append_string_small_to_small);
    tcase_add_test(tc, string_append_string_large_to_large);
    tcase_add_test(tc, string_append_string_small_to_large);
    tcase_add_test(tc, string_append_cstr_part_succeeds);
    tcase_add_test(tc, string_append_string_part_succeeds);
    tcase_add_test(tc, string_append_self_whole);
    tcase_add_test(tc, string_append_self_part);
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
    tcase_add_test(tc, string_trim_string_both);
    tcase_add_test(tc, string_trim_string_left);
    tcase_add_test(tc, string_trim_string_right);
    tcase_add_test(tc, string_trim_string_neither);
    tcase_add_test(tc, string_trim_string_multiple_letters);
    tcase_add_test(tc, string_trim_string_full_sequence_only);
    tcase_add_test(tc, string_trim_any_string_single);
    tcase_add_test(tc, string_trim_any_string_multiple);
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
    tcase_add_test(tc, string_replace_self_large_to_small_substring_before_replace);
    tcase_add_test(tc, string_replace_self_large_to_small_substring_after_replace);
    tcase_add_test(tc, string_replace_self_large_to_small_overlapping);
    tcase_add_test(tc, string_replace_self_small_to_large_before_replace);
    tcase_add_test(tc, string_replace_self_small_to_large_after_replace);
    tcase_add_test(tc, string_replace_self_small_to_large_overlapping_whole);
    tcase_add_test(tc, string_replace_self_small_to_large_overlapping_right);
    tcase_add_test(tc, string_replace_self_small_to_large_overlapping_left);
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
    tcase_add_test(tc, string_join_string_none);
    tcase_add_test(tc, string_join_string_two);
    tcase_add_test(tc, string_join_string_many);
    tcase_add_test(tc, string_join_refs_string_none);
    tcase_add_test(tc, string_join_refs_string_two);
    tcase_add_test(tc, string_join_refs_string_many);
    tcase_add_test(tc, string_format_string_new);
    tcase_add_test(tc, string_format_string_existing);
    tcase_add_test(tc, string_format_string_append);
    tcase_add_test(tc, string_format_cstr_new);
    tcase_add_test(tc, string_format_cstr_existing);
    tcase_add_test(tc, string_format_cstr_append);
    tcase_add_test(tc, string_split_string_init_into_existing_less_than_size);
    tcase_add_test(tc, string_split_string_init_into_existing_equal_size);
    tcase_add_test(tc, string_split_string_init_into_existing_greater_than_size);
    tcase_add_test(tc, string_split_string_no_init_into_existing_less_than_size);
    tcase_add_test(tc, string_split_string_no_init_into_existing_equal_size);
    tcase_add_test(tc, string_split_string_no_init_into_existing_greater_than_size);
    tcase_add_test(tc, string_split_string_allocate_init);
    tcase_add_test(tc, string_split_string_skip_empty);
    tcase_add_test(tc, string_split_string_dont_skip_empty);
    tcase_add_test(tc, string_format_time_string_new);
    tcase_add_test(tc, string_format_time_string_existing);
    tcase_add_test(tc, string_format_time_string_append);
    tcase_add_test(tc, string_format_time_cstr_new);
    tcase_add_test(tc, string_format_time_cstr_existing);
    tcase_add_test(tc, string_format_time_cstr_append);
    tcase_add_test(tc, string_hash_verify);

    tcase_add_test(tc, string_init_fail_null_arg);
    tcase_add_test(tc, string_init_fail_oom);
    tcase_add_test(tc, string_init_size_fail_null_arg);
    tcase_add_test(tc, string_init_size_fail_oob);
    tcase_add_test(tc, string_init_size_fail_oom);
    tcase_add_test(tc, string_u8_codepoints_fail_null_arg);
    tcase_add_test(tc, string_u8_get_with_size_fail_null_arg);

    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}