#include <sso_string.h>

int main(void) {
    FILE* file = fopen("test.txt", "r");
    String line = string_create("");
    char buffer[10];
    struct StringFileReadState read_state;
    string_file_read_state_init(&read_state, buffer, sizeof(buffer));
    while(string_file_read_line(&line, file, &read_state)) {
        puts(string_data(&line));
    }

    string_free_resources(&line);
    printf("Result: %d", read_state.result);

    return 0;
}