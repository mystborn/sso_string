#include <stdio.h>

#include "prc_string.h"

int main(int argc, char **argv) {
    String str;
    string_init(&str, "i");
    if(___string_is_long(&str)) {
        puts("String is falsely large.");
    } else {
        puts("String is correctly short.");
    }
    printf("Size: %llu\n", string_size(&str));
    string_free_resources(&str);
    string_init(&str, "abcdefghijklmnopqrstuvwxyz");
    if(___string_is_long(&str)) {
        puts("String is correctly large.");
    } else {
        puts("String is falsely short.");
    }
    printf("Size: %llu, Capacity: %llu\n", string_size(&str), string_capacity(&str));
    printf("%llu, %llu\n", str.l.size, str.l.cap);
    string_free_resources(&str);


    return 0;
}