#include <check.h>

#include <stdio.h>

#include "../include/sso_string.h"

static String small;
static String large;

#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

static void string_setup(void) {
    string_init(&small, "hello");
}

static void string_teardown(void) {
    string_free_resources(&small);
}

static void string_start(void) {
    string_init(&large, ALPHABET);
}

static void string_reset(void) {
    string_free_resources(&large);
}


START_TEST(short_string_has_short_flag) {
    ck_assert(!___sso_string_is_long(&small));
}
END_TEST

START_TEST(large_string_has_long_flag) {
    ck_assert(___sso_string_is_long(&large));
}
END_TEST

START_TEST(string_init_from_substring_less_than_length) {
    String str;
    string_init_size(&str, "moooo", 3);
    ck_assert(strcmp(string_cstr(&str), "moo") == 0);
    string_free_resources(&str);

    string_init_size(&str, ALPHABET, 24);
    ck_assert(strcmp(string_cstr(&str), "abcdefghijklmnopqrstuvwx") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_init_from_substring_equal_to_length) {
    String str;
    string_init_size(&str, "moo", 3);
    ck_assert(strcmp(string_cstr(&str), "moo") == 0);
    string_free_resources(&str);

    string_init_size(&str, ALPHABET, 26);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_size_short) {
    ck_assert(string_size(&small) == 5);
}
END_TEST

START_TEST(string_size_long) {
    ck_assert(string_size(&large) == 26);
}
END_TEST

START_TEST(string_capacity_short) {
    ck_assert(string_capacity(&small) == ___sso_string_min_cap);
}
END_TEST

START_TEST(string_capacity_long) {
    ck_assert(string_capacity(&large) == 26);
    String str = string_create(ALPHABET ALPHABET);
    ck_assert(string_capacity(&str) == 52);
    string_append(&str, "z");
    ck_assert(string_capacity(&str) > 52);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_get_short) {
    ck_assert(string_get(&small, 1) == 'e');
}
END_TEST;

START_TEST(string_get_long) {
    for(int i = 0; i < 26; i++)
        ck_assert(string_get(&large, i) == 'a' + i);
}
END_TEST

START_TEST(string_set_short) {
    String str = string_create("hello");
    string_set(&str, 2, 'n');
    ck_assert(strcmp(string_cstr(&str), "henlo") == 0);
}
END_TEST

START_TEST(string_set_long) {
    String str = string_create(ALPHABET);
    string_set(&str, 7, 'a');
    ck_assert(strcmp(string_cstr(&str), "abcdefgaijklmnopqrstuvwxyz") == 0);
}
END_TEST

START_TEST(string_empty_short) {
    String str = string_create("");
    ck_assert(!string_empty(&small));
    ck_assert(string_empty(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_empty_long) {
    String str = string_create(ALPHABET);
    string_clear(&str);
    ck_assert(!string_empty(&large));
    ck_assert(string_empty(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_reserve_short_to_short) {
    String str = string_create("hello");
    string_reserve(&str, 1);
    ck_assert(!___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_reserve_short_to_long) {
    String str = string_create("hello");
    string_reserve(&str, 26);
    ck_assert(___sso_string_is_long(&str));
    ck_assert(string_capacity(&str) >= 26);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_reserve_long_to_bigger) {
    String str = string_create(ALPHABET);
    ck_assert(string_capacity(&str) < 40);
    string_reserve(&str, 40);
    ck_assert(string_capacity(&str) >= 40);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_shrink_long_to_long) {
    String str = string_create(ALPHABET);
    string_append(&str, ALPHABET);
    size_t size = string_size(&str);
    ck_assert(string_capacity(&str) != size);
    string_shrink_to_fit(&str);
    ck_assert(string_capacity(&str) == size);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_shrink_long_to_short) {
    String str = string_create("hello");
    string_reserve(&str, 26);
    ck_assert(___sso_string_is_long(&str));
    string_shrink_to_fit(&str);
    ck_assert(!___sso_string_is_long(&str));
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_clear_short) {
    String str = string_create("hello");
    string_clear(&str);
    ck_assert(strcmp(string_cstr(&str), "") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_clear_long) {
    String str = string_create(ALPHABET);
    string_clear(&str);
    ck_assert(strcmp(string_cstr(&str), "") == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_cstr_small_to_small) {
    String str = string_create("rok");
    string_insert(&str, "c", 2);
    ck_assert(strcmp(string_cstr(&str), "rock") == 0);
    ck_assert(!___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_cstr_small_to_large) {
    String str = string_create("abz");
    string_insert(&str, "cdefghijklmnopqrstuvwxy", 2);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_cstr_large_to_large) {
    String str = string_create("abcdefghijklmnopqrstuvyz");
    string_insert(&str, "wx", 22);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_insert_string_small_to_small) {
    String str = string_create("rok");
    String value = string_create("c");
    string_insert_string(&str, &value, 2);
    ck_assert(strcmp(string_cstr(&str), "rock") == 0);
    ck_assert(!___sso_string_is_long(&str));
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_insert_string_small_to_large) {
    String str = string_create("abcz");
    String value = string_create("defghijklmnopqrstuvwxy");
    string_insert_string(&str, &value, 3);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_insert_string_large_to_large) {
    String str = string_create("adefghijklmnopqrstuvwxyz");
    String value = string_create("bc");
    string_insert_string(&str, &value, 1);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_erase_small) {
    String str = string_create("caw, caw");
    string_erase(&str, 3, 2);
    ck_assert(strcmp(string_cstr(&str), "cawcaw") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_erase_large) {
    String str = string_create(ALPHABET);
    string_erase(&str, 3, 3);
    ck_assert(strcmp(string_cstr(&str), "abcghijklmnopqrstuvwxyz") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_push_back_small) {
    String str = string_create("roc");
    string_push_back(&str, 'k');
    ck_assert(strcmp(string_cstr(&str), "rock") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_push_back_large) {
    String str = string_create("abcdefghijklmnopqrstuvwxy");
    string_push_back(&str, 'z');
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_push_back_small_to_large) {
    String str = string_create("");
    for(int i = 0; i < 26; i++) {
        string_push_back(&str, ('a' + i));
    }
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_pop_back_small) {
    String str = string_create("hello");
    ck_assert(string_pop_back(&str) == 'o');
    ck_assert(string_pop_back(&str) == 'l');
    string_free_resources(&str);
}
END_TEST

START_TEST(string_pop_back_large) {
    String str = string_create(ALPHABET);
    for(int i = 25; i >= 0; --i)
        ck_assert(string_pop_back(&str) == 'a' + i);
}
END_TEST

START_TEST(string_pop_back_empty) {
    String str = string_create("");
    ck_assert(string_pop_back(&str) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_cstr_small_to_small) {
    String str = string_create("hel");
    string_append(&str, "lo");
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_cstr_large_to_large) {
    String str = string_create("abcdefghijklmnopqrstuvwx");
    string_append(&str, "yz");
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_cstr_small_to_large) {
    String str = string_create("abc");
    string_append(&str, "defghijklmnopqrstuvwxyz");
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_append_string_small_to_small) {
    String str = string_create("hel");
    String value = string_create("lo");
    string_append_string(&str, &value);
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_append_string_large_to_large) {
    String str = string_create("abcdefghijklmnopqrstuvwx");
    String value = string_create("yz");
    string_append_string(&str, &value);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_append_string_small_to_large) {
    String str = string_create("abc");
    String value = string_create("defghijklmnopqrstuvwxyz");
    string_append_string(&str, &value);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
    string_free_resources(&value);
}
END_TEST

START_TEST(string_compare_cstr_large_and_small) {
    ck_assert(string_compare(&small, "hello") == 0);
    ck_assert(string_compare(&small, "helloo") == -1);
    ck_assert(string_compare(&small, "hell") == 1);
    ck_assert(string_compare(&large, ALPHABET) == 0);
    ck_assert(string_compare(&large, "abcdefghijklmnopqrstuvwxyzz") == -1);
    ck_assert(string_compare(&large, "abcdefghijklmnopqrstuvwxy") == 1);
}
END_TEST

START_TEST(string_compare_string_large_and_small) {
    String hello = string_create("hello");
    String hell = string_create("hell");
    String helloo = string_create("helloo");
    String large_eq = string_create(ALPHABET);
    String large_gt = string_create(ALPHABET "z");
    String large_lt;
    string_init_size(&large_lt, ALPHABET, 25);

    ck_assert(string_compare_string(&small, &hello) == 0);
    ck_assert(string_compare_string(&small, &helloo) == -1);
    ck_assert(string_compare_string(&small, &hell) == 1);
    ck_assert(string_compare_string(&large, &large_eq) == 0);
    ck_assert(string_compare_string(&large, &large_gt) == -1);
    ck_assert(string_compare_string(&large, &large_lt) == 1);

    string_free_resources(&hello);
    string_free_resources(&hell);
    string_free_resources(&helloo);
    string_free_resources(&large_eq);
    string_free_resources(&large_gt);
    string_free_resources(&large_lt);
}
END_TEST

START_TEST(string_starts_with_cstr_large_and_small) {
    ck_assert(string_starts_with(&small, "he"));
    ck_assert(string_starts_with(&small, "hello"));
    ck_assert(!string_starts_with(&small, "me"));
    ck_assert(string_starts_with(&large, "abc"));
    ck_assert(string_starts_with(&large, ALPHABET));
    ck_assert(!string_starts_with(&large, "bac"));
}
END_TEST

START_TEST(string_starts_with_string_large_and_small) {
    String small_first_letters = string_create("he");
    String small_whole_word = string_create("hello");
    String small_invalid = string_create("me");
    String large_first_letters = string_create("abc");
    String large_whole_word = string_create(ALPHABET);
    String large_invalid = string_create("bac");

    ck_assert( string_starts_with_string(&small, &small_first_letters));
    ck_assert( string_starts_with_string(&small, &small_whole_word));
    ck_assert(!string_starts_with_string(&small, &small_invalid));
    ck_assert( string_starts_with_string(&large, &large_first_letters));
    ck_assert( string_starts_with_string(&large, &large_whole_word));
    ck_assert(!string_starts_with_string(&large, &large_invalid));

    string_free_resources(&small_first_letters);
    string_free_resources(&small_whole_word);
    string_free_resources(&small_invalid);
    string_free_resources(&large_first_letters);
    string_free_resources(&large_whole_word);
    string_free_resources(&large_invalid);
}
END_TEST

START_TEST(string_ends_with_cstr_large_and_small) {
    ck_assert( string_ends_with(&small, "lo"));
    ck_assert( string_ends_with(&small, "hello"));
    ck_assert(!string_ends_with(&small, "no"));
    ck_assert( string_ends_with(&large, "xyz"));
    ck_assert( string_ends_with(&large, ALPHABET));
    ck_assert(!string_ends_with(&large, "abc"));
}
END_TEST

START_TEST(string_ends_with_string_large_and_small) {
    String small_last_letters = string_create("lo");
    String small_whole_word = string_create("hello");
    String small_invalid = string_create("no");
    String large_last_letters = string_create("xyz");
    String large_whole_word = string_create(ALPHABET);
    String large_invalid = string_create("abc");

    ck_assert( string_ends_with_string(&small, &small_last_letters));
    ck_assert( string_ends_with_string(&small, &small_whole_word));
    ck_assert(!string_ends_with_string(&small, &small_invalid));
    ck_assert( string_ends_with_string(&large, &large_last_letters));
    ck_assert( string_ends_with_string(&large, &large_whole_word));
    ck_assert(!string_ends_with_string(&large, &large_invalid));
    
    string_free_resources(&small_last_letters);
    string_free_resources(&small_whole_word);
    string_free_resources(&small_invalid);
    string_free_resources(&large_last_letters);
    string_free_resources(&large_whole_word);
    string_free_resources(&large_invalid);
}
END_TEST

START_TEST(string_replace_cstr_small_same_length) {
    String str = string_create("hrklo");
    string_replace(&str, 1, 2, "el");
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_cstr_small_less_length) {
    String str = string_create("heckno");
    string_replace(&str, 2, 3, "ll");
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_cstr_small_greater_length) {
    String str = string_create("hko");
    string_replace(&str, 1, 1, "ell");
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_cstr_small_to_large) {
    String str = string_create("abcabc");
    string_replace(&str, 3, 3, "defghijklmnopqrstuvwxyz");
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_cstr_large) {
    String str = string_create(ALPHABET);
    string_replace(&str, 5, 1, "moo");
    ck_assert(strcmp(string_cstr(&str), "abcdemooghijklmnopqrstuvwxyz") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_small_same_length) {
    String str = string_create("hrklo");
    String value = string_create("el");
    string_replace_string(&str, 1, 2, &value);
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_small_less_length) {
    String str = string_create("heckno");
    String value = string_create("ll");
    string_replace_string(&str, 2, 3, &value);
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_small_greater_length) {
    String str = string_create("hko");
    String value = string_create("ell");
    string_replace_string(&str, 1, 1, &value);
    ck_assert(strcmp(string_cstr(&str), "hello") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_small_to_large) {
    String str = string_create("abcabc");
    String value = string_create("defghijklmnopqrstuvwxyz");
    string_replace_string(&str, 3, 3, &value);
    ck_assert(strcmp(string_cstr(&str), ALPHABET) == 0);
    ck_assert(___sso_string_is_long(&str));
    string_free_resources(&str);
}
END_TEST

START_TEST(string_replace_string_large) {
    String str = string_create(ALPHABET);
    String value = string_create("moo");
    string_replace_string(&str, 5, 1, &value);
    ck_assert(strcmp(string_cstr(&str), "abcdemooghijklmnopqrstuvwxyz") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_substring_part) {
    String result;
    string_substring(&large, 3, 6, &result);
    ck_assert(string_compare(&result, "defghi") == 0);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_substring_whole) {
    String result;
    string_substring(&small, 0, 5, &result);
    ck_assert(string_compare_string(&small, &result) == 0);
    string_free_resources(&result);
}
END_TEST

START_TEST(string_copy_to_part) {
    char buffer[3];
    buffer[2] = 0;
    string_copy_to(&small, buffer, 1, 2);
    ck_assert(strcmp(buffer, "el") == 0);
}
END_TEST

START_TEST(string_copy_to_whole) {
    char buffer[27];
    buffer[26] = 0;
    string_copy_to(&large, buffer, 0, 26);
    ck_assert(string_compare(&large, buffer) == 0);
}
END_TEST

START_TEST(string_resize_smaller) {
    String str = string_create("moooooo");
    string_resize(&str, 3, 0);
    ck_assert(string_compare(&str, "moo") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_resize_bigger) {
    String str = string_create("m");
    string_resize(&str, 3, 'o');
    ck_assert(string_compare(&str, "moo") == 0);
    string_free_resources(&str);
}
END_TEST

START_TEST(string_swap_small_to_small) {
    String left = string_create("hello");
    String right = string_create("moo");
    string_swap(&left, &right);
    ck_assert(string_compare(&left, "moo") == 0);
    ck_assert(string_compare(&right, "hello") == 0);
    string_free_resources(&left);
    string_free_resources(&right);
}
END_TEST

START_TEST(string_swap_large_to_large) {
    char buffer[] = "This is a very long sentence that is over the max length";
    String left = string_create(buffer);
    String right = string_create(ALPHABET);
    string_swap(&left, &right);
    ck_assert(string_compare(&left, ALPHABET) == 0);
    ck_assert(string_compare(&right, buffer) == 0);
    string_free_resources(&left);
    string_free_resources(&right);
}
END_TEST

START_TEST(string_swap_small_to_large) {
    String left = string_create("hello");
    String right = string_create(ALPHABET);
    string_swap(&left, &right);
    ck_assert(string_compare(&left, ALPHABET) == 0);
    ck_assert(string_compare(&right, "hello") == 0);
    string_free_resources(&left);
    string_free_resources(&right);
}
END_TEST

START_TEST(string_find_cstr_part_valid) {
    ck_assert(string_find(&small, 0, "l") == 2);
    ck_assert(string_find(&large, 0, "lmnop") == 11);
    ck_assert(string_find(&small, 3, "l") == 3);
}
END_TEST

START_TEST(string_find_cstr_whole_valid) {
    ck_assert(string_find(&small, 0, "hello") == 0);
    ck_assert(string_find(&large, 0, ALPHABET) == 0);
}
END_TEST

START_TEST(string_find_cstr_invalid) {
    ck_assert(string_find(&small, 0, "moo") == SIZE_MAX);
    ck_assert(string_find(&large, 0, "bac") == SIZE_MAX);
}
END_TEST

START_TEST(string_find_string_part_valid) {
    String small_value = string_create("l");
    String large_value = string_create("lmnop");
    ck_assert(string_find_string(&small, 0, &small_value) == 2);
    ck_assert(string_find_string(&large, 0, &large_value) == 11);
    ck_assert(string_find_string(&small, 3, &small_value) == 3);
}
END_TEST

START_TEST(string_find_string_whole_valid) {
    String small_value = string_create("hello");
    String large_value = string_create(ALPHABET);
    ck_assert(string_find_string(&small, 0, &small_value) == 0);
    ck_assert(string_find_string(&large, 0, &large_value) == 0);
}
END_TEST

START_TEST(string_find_string_invalid) {
    String small_value = string_create("moo");
    String large_value = string_create("bac");
    ck_assert(string_find_string(&small, 0, &small_value) == SIZE_MAX);
    ck_assert(string_find_string(&large, 0, &large_value) == SIZE_MAX);
}
END_TEST

START_TEST(string_rfind_cstr_part_valid) {
    ck_assert(string_rfind(&small, 4, "l") == 3);
    ck_assert(string_rfind(&large, 25, "lmnop") == 11);
    ck_assert(string_rfind(&small, 2, "l") == 2);
}
END_TEST

START_TEST(string_rfind_cstr_whole_valid) {
    ck_assert(string_rfind(&small, 4, "hello") == 0);
    ck_assert(string_rfind(&large, 25, ALPHABET) == 0);
}
END_TEST

START_TEST(string_rfind_cstr_invalid) {
    ck_assert(string_rfind(&small, 4, "moo") == SIZE_MAX);
    ck_assert(string_rfind(&large, 25, "bac") == SIZE_MAX);
}
END_TEST

START_TEST(string_rfind_string_part_valid) {
    String small_value = string_create("l");
    String large_value = string_create("lmnop");
    ck_assert(string_rfind_string(&small, 4, &small_value) == 3);
    ck_assert(string_rfind_string(&large, 25, &large_value) == 11);
    ck_assert(string_rfind_string(&small, 2, &small_value) == 2);
}
END_TEST

START_TEST(string_rfind_string_whole_valid) {
    String small_value = string_create("hello");
    String large_value = string_create(ALPHABET);
    ck_assert(string_rfind_string(&small, 4, &small_value) == 0);
    ck_assert(string_rfind_string(&large, 25, &large_value) == 0);
}
END_TEST

START_TEST(string_rfind_string_invalid) {
    String small_value = string_create("moo");
    String large_value = string_create("bac");
    ck_assert(string_rfind_string(&small, 4, &small_value) == SIZE_MAX);
    ck_assert(string_rfind_string(&large, 25, &large_value) == SIZE_MAX);
}
END_TEST

int main(void) {
    int number_failed;

    Suite* s = suite_create("String");
    TCase* tc = tcase_create("String");

    tcase_add_checked_fixture(tc, string_start, string_reset);
    tcase_add_unchecked_fixture(tc, string_setup, string_teardown);

    tcase_add_test(tc, short_string_has_short_flag);
    tcase_add_test(tc, large_string_has_long_flag);
    tcase_add_test(tc, string_init_from_substring_less_than_length);
    tcase_add_test(tc, string_init_from_substring_equal_to_length);
    tcase_add_test(tc, string_size_short);
    tcase_add_test(tc, string_size_long);
    tcase_add_test(tc, string_capacity_short);
    tcase_add_test(tc, string_capacity_long);
    tcase_add_test(tc, string_get_short);
    tcase_add_test(tc, string_get_long);
    tcase_add_test(tc, string_set_short);
    tcase_add_test(tc, string_set_long);
    tcase_add_test(tc, string_empty_short);
    tcase_add_test(tc, string_empty_long);
    tcase_add_test(tc, string_reserve_short_to_short);
    tcase_add_test(tc, string_reserve_short_to_long);
    tcase_add_test(tc, string_reserve_long_to_bigger);
    tcase_add_test(tc, string_shrink_long_to_long);
    tcase_add_test(tc, string_shrink_long_to_short);
    tcase_add_test(tc, string_clear_short);
    tcase_add_test(tc, string_clear_long);
    tcase_add_test(tc, string_insert_cstr_small_to_small);
    tcase_add_test(tc, string_insert_cstr_small_to_large);
    tcase_add_test(tc, string_insert_cstr_large_to_large);
    tcase_add_test(tc, string_insert_string_small_to_small);
    tcase_add_test(tc, string_insert_string_small_to_large);
    tcase_add_test(tc, string_insert_string_large_to_large);
    tcase_add_test(tc, string_erase_small);
    tcase_add_test(tc, string_erase_large);
    tcase_add_test(tc, string_push_back_small);
    tcase_add_test(tc, string_push_back_large);
    tcase_add_test(tc, string_push_back_small_to_large);
    tcase_add_test(tc, string_pop_back_small);
    tcase_add_test(tc, string_pop_back_large);
    tcase_add_test(tc, string_pop_back_empty);
    tcase_add_test(tc, string_append_cstr_small_to_small);
    tcase_add_test(tc, string_append_cstr_large_to_large);
    tcase_add_test(tc, string_append_cstr_small_to_large);
    tcase_add_test(tc, string_append_string_small_to_small);
    tcase_add_test(tc, string_append_string_large_to_large);
    tcase_add_test(tc, string_append_string_small_to_large);
    tcase_add_test(tc, string_compare_cstr_large_and_small);
    tcase_add_test(tc, string_compare_string_large_and_small);
    tcase_add_test(tc, string_starts_with_cstr_large_and_small);
    tcase_add_test(tc, string_starts_with_string_large_and_small);
    tcase_add_test(tc, string_ends_with_cstr_large_and_small);
    tcase_add_test(tc, string_ends_with_string_large_and_small);
    tcase_add_test(tc, string_replace_cstr_small_same_length);
    tcase_add_test(tc, string_replace_cstr_small_less_length);
    tcase_add_test(tc, string_replace_cstr_small_greater_length);
    tcase_add_test(tc, string_replace_cstr_small_to_large);
    tcase_add_test(tc, string_replace_cstr_large);
    tcase_add_test(tc, string_replace_string_small_same_length);
    tcase_add_test(tc, string_replace_string_small_less_length);
    tcase_add_test(tc, string_replace_string_small_greater_length);
    tcase_add_test(tc, string_replace_string_small_to_large);
    tcase_add_test(tc, string_replace_string_large);
    tcase_add_test(tc, string_substring_part);
    tcase_add_test(tc, string_substring_whole);
    tcase_add_test(tc, string_copy_to_part);
    tcase_add_test(tc, string_copy_to_whole);
    tcase_add_test(tc, string_resize_smaller);
    tcase_add_test(tc, string_resize_bigger);
    tcase_add_test(tc, string_swap_small_to_small);
    tcase_add_test(tc, string_swap_large_to_large);
    tcase_add_test(tc, string_swap_small_to_large);
    tcase_add_test(tc, string_find_cstr_part_valid);
    tcase_add_test(tc, string_find_cstr_whole_valid);
    tcase_add_test(tc, string_find_cstr_invalid);
    tcase_add_test(tc, string_find_string_part_valid);
    tcase_add_test(tc, string_find_string_whole_valid);
    tcase_add_test(tc, string_find_string_invalid);
    tcase_add_test(tc, string_rfind_cstr_part_valid);
    tcase_add_test(tc, string_rfind_cstr_whole_valid);
    tcase_add_test(tc, string_rfind_cstr_invalid);
    tcase_add_test(tc, string_rfind_string_part_valid);
    tcase_add_test(tc, string_rfind_string_whole_valid);
    tcase_add_test(tc, string_rfind_string_invalid);

    suite_add_tcase(s, tc);

    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}