#include <sso_string.h>

/*
    A (really messy) program that will create all of the basic parts
    of a function documentation file for each function in the
    sso_string.h header file.

    Examples still need to be handwritten, but if they exist they won't be overwritten
    by this file.

    This file contains minimal error checking since I am mostly unconcerned about 
    running out of memory.
*/

typedef enum Declaration {
    DECLARATION_FUNCTION_PROTOTYPE,
    DECLARATION_MACRO,
    DECLARATION_NONE
} Declaration;


/**
    Defines the parts of a doc-string using in sso_string.
*/
typedef struct DocParts {
    String* brief;
    String** params;
    String* ret;
    String* remarks;
    String** see_also;
    size_t params_count;
    size_t params_capacity;
    size_t see_also_count;
    size_t see_also_capacity;
} DocParts;

void doc_parts_free(DocParts* parts) {
    if(!parts)
        return;
    string_free(parts->brief);
    string_free(parts->ret);
    string_free(parts->remarks);

    for(size_t i = 0; i < parts->params_count; i++)
        string_free(parts->params[i]);

    free(parts->params);

    for(size_t i = 0; i < parts->see_also_count; i++)
        string_free(parts->see_also[i]);

    free(parts->see_also);
    free(parts);
}

bool read_next_docstring(String* doc_string, FILE* file, struct StringFileReadState* read_state) {
    string_clear(doc_string);
    String line = string_create("");
    while(string_file_read_line(&line, file, read_state)) {
        if(string_starts_with(&line, "/**")) {
            string_append(doc_string, &line);
            string_append(doc_string, "\n");
            break;
        }
    }

    if(feof(file) || ferror(file))
        goto error;

    do {
        if(!string_file_read_line(&line, file, read_state))
            break;

        string_append(doc_string, &line);
        if(string_starts_with(&line, "*/"))
            break;
        string_append(doc_string, "\n");
    }
    while(!feof(file) && !ferror(file));

    if(feof(file) || ferror(file))
        goto error;

    string_free_resources(&line);
    return true;

    error:
        string_free_resources(&line);
        return false;
}

bool read_function_prototype(String* prototype, String* line, FILE* file, struct StringFileReadState* read_state) {
    while(!feof(file) && !ferror(file)) {
        string_trim(line, " ");
        string_append(prototype, line);

        if(string_ends_with(line, ";"))
            break;

        if(!string_ends_with(line, "("))
            string_append(prototype, " ");
        string_file_read_line(line, file, read_state);
    }

    if(feof(file) || ferror(file))
        return false;

    return true;
}

bool read_macro(String* macro, String* line, FILE* file, struct StringFileReadState* read_state) {
    bool first = true;
    bool has_more_lines = false;
    do {
        has_more_lines = string_ends_with(line, "\\");
        if(has_more_lines)
            string_trim_end(line, "\\");
        
        string_trim(line, " ");

        if(first) {
            first = false;
        } else {
            string_append(macro, "\n    ");
        }

        string_append(macro, line);

        if(has_more_lines)
            string_file_read_line(line, file, read_state);
    }
    while(has_more_lines && !feof(file) && !ferror(file));

    if(feof(file) || ferror(file))
        return false;

    return true;
}

bool read_declaration(String* declaration, Declaration* declaration_type, FILE* file, struct StringFileReadState* read_state) {
    string_clear(declaration);
    String line = STRING_EMPTY;
    string_file_read_line(&line, file, read_state);

    if (string_starts_with(&line, "SSO_STRING_EXPORT ")) {
        string_trim_start(&line, "SSO_STRING_EXPORT ");
        *declaration_type = DECLARATION_FUNCTION_PROTOTYPE;
    } else if (string_starts_with(&line, "static inline ")) {
        string_trim_start(&line, "static inline ");
        *declaration_type = DECLARATION_FUNCTION_PROTOTYPE;
    } else if(string_starts_with(&line, "#define ")) {
        *declaration_type = DECLARATION_MACRO;
    } else {
        *declaration_type = DECLARATION_NONE;
    }

    bool success;

    switch(*declaration_type) {
        case DECLARATION_FUNCTION_PROTOTYPE:
            success = read_function_prototype(declaration, &line, file, read_state);
            break;
        case DECLARATION_MACRO:
            success = read_macro(declaration, &line, file, read_state);
            break;
        default:
            success = false;
            break;
    }

    string_free_resources(&line);
    return success;
}

void extract_function_name(String* prototype, String* name) {
    size_t param_start = string_find(prototype, 0, "(");
    size_t name_start = string_rfind(prototype, string_size(prototype) - param_start, " ") + 1;
    string_substring(prototype, name_start, param_start - name_start, name);
}

void extract_macro_name(String* macro, String* name) {
    size_t start = string_find(macro, 0, " ") + 1;
    size_t space = string_find(macro, start, " ");
    size_t newline = string_find(macro, start, "\n");
    size_t param_start = string_find(macro, start, "(");

    size_t end = SIZE_MAX;
    end = min(space, min(newline, param_start));

    string_substring(macro, start, end - start, name);
}

void read_tags(String* tags, FILE* file, struct StringFileReadState* read_state) {
    if(string_file_read_line(tags, file, read_state)) {
        if(!string_starts_with(tags, "tags:"))
            string_clear(tags);
    }
}

void read_example(String* example, FILE* file, struct StringFileReadState* read_state) {
    String line = string_create("");
    bool found = false;
    while(string_file_read_line(&line, file, read_state)) {
        if(found || string_starts_with(&line, "## Example")) {
            string_append(example, &line);
            string_append(example, "\n");
            found = true;
        }
    }

    string_free_resources(&line);
}

DocParts* split_doc_parts(String* doc_string) {
    DocParts* parts = calloc(1, sizeof(*parts));
    
    int line_count;
    String* lines = string_split(doc_string, "\n", NULL, STRING_SPLIT_ALLOCATE, &line_count, false, true);
    for(int i = 0; i < line_count; i++)
        string_trim(lines + i, " ");
    int line = 1;

    parts->brief = string_create_ref("");
    while(line < line_count && !string_starts_with(lines + line, "*/") && !string_starts_with(lines + line, "@")) {
        if(string_u8_is_null_or_whitespace(lines + line) && !string_ends_with(parts->brief, "\n")) {
            string_append(parts->brief, "\n\n");
        } else {
            if (string_size(parts->brief) > 0 && !string_ends_with(parts->brief, "\n"))
                string_append(parts->brief, " ");
            string_append(parts->brief, lines + line);
        }

        line++;
    }

    string_trim_end(parts->brief, "\n");

    String* last_string = NULL;
    while(line < line_count && !string_starts_with(lines + line, "*/")) {
        if(last_string != NULL && string_u8_is_null_or_whitespace(lines + line)) {
            string_append(last_string, "\n\n");
            line++;
            continue;
        }
        if (string_starts_with(lines + line, "@")) {
            if(last_string)
                string_trim_end(last_string, "\n");

            if(string_starts_with(lines + line, "@param")) {
                if(parts->params_count == parts->params_capacity) {
                    parts->params_capacity = parts->params_capacity == 0 ? 2 : parts->params_capacity * 2;
                    parts->params = realloc(parts->params, parts->params_capacity * sizeof(*parts->params));
                }

                parts->params[parts->params_count++] = last_string = string_create_ref("");
            } else if(string_starts_with(lines + line, "@return")) {
                string_trim_start(lines + line, "@return ");
                parts->ret = last_string = string_create_ref("");
            } else if(string_starts_with(lines + line, "@remarks")) {
                string_trim_start(lines + line, "@remarks ");
                parts->remarks = last_string = string_create_ref("");
            } else if(string_starts_with(lines + line, "@see")) {
                string_trim_start(lines + line, "@see ");
                if(parts->see_also_count == parts->see_also_capacity) {
                    parts->see_also_capacity = parts->see_also_capacity == 0 ? 2 : parts->see_also_capacity * 2;
                    parts->see_also = realloc(parts->see_also, parts->see_also_capacity * sizeof(*parts->see_also));
                }

                parts->see_also[parts->see_also_count++] = last_string = string_create_ref("");
            }
        }

        if (string_size(last_string) > 0 && !string_ends_with(last_string, "\n"))
            string_append(last_string, " ");
        string_append(last_string, lines + line);
        line++;
    }

    for(int i = 0; i < line_count; i++)
        string_free_resources(lines + i);
    free(lines);

    return parts;
}

void write_function_doc_file(String* doc_string, String* prototype, struct StringFileReadState* read_state) {
    String name = string_create("");
    String example = string_create("");
    String tags = string_create("");
    String param_string = string_create("");
    String space = string_create(" ");
    String* fname = NULL;
    DocParts* doc_parts = NULL;
    FILE* file = NULL;
    String* param_type_list = NULL;
    int param_count;

    extract_function_name(prototype, &name);
    doc_parts = split_doc_parts(doc_string);

    size_t param_start = string_find(prototype, 0, "(") + 1;
    size_t param_end = string_find(prototype, 0, ")");
    string_substring(prototype, param_start, param_end - param_start, &param_string);
    param_type_list = string_split(&param_string, ", ", NULL, STRING_SPLIT_ALLOCATE, &param_count, true, true);
    for(int i = 0; i < param_count; i++) {
        size_t type_end = string_find_cstr(param_type_list + i, 0, " ");
        string_erase(param_type_list + i, type_end, string_size(param_type_list + i) - type_end);
    }

    fname = string_format(NULL, "functions/%s.md", string_data(&name));

    file = fopen(string_data(fname), "r+");

    if(file) {
        read_tags(&tags, file, read_state);
        read_example(&example, file, read_state);
        fclose(file);
        remove(string_data(fname));
    }

    file = fopen(string_data(fname), "w");
    if(!file)
        goto error;

    if(!string_is_null_or_empty(&tags))
        fprintf(file, "%s\n\n", string_data(&tags));
    
    fprintf(file, "# %s\n\n", string_data(&name));

    if(doc_parts->brief)
        fprintf(file, "%s\n\n", string_data(doc_parts->brief));

    fprintf(file, "## Syntax\n\n```c\n%s\n```\n\n", string_data(prototype));

    if(param_count > 0 && !string_equals(param_type_list, "void")) {
        fprintf(file, "| Name | Type | Description |\n");
        fprintf(file, "| --- | --- | --- |\n");
        String param_name = string_create("");
        String description = string_create("");
        for(int i = 0; i < param_count; i++) {
            string_clear(&param_name);
            string_clear(&description);

            // Remove @param from the string.
            string_erase(doc_parts->params[i], 0, 7);

            size_t name_end = string_find(doc_parts->params[i], 0, " ");
            string_substring(doc_parts->params[i], 0, name_end, &param_name);
            string_substring(doc_parts->params[i], name_end + 1, string_size(doc_parts->params[i]) - name_end - 1, &description);

            fprintf(file, "| %s | %s | %s |\n", string_data(&param_name), string_data(param_type_list + i), string_data(&description));
        }

        fprintf(file, "\n");
    }

    if(doc_parts->ret) {
        fprintf(file, "**Returns:** %s\n\n", string_data(doc_parts->ret));
    }

    if(doc_parts->remarks) {
        fprintf(file, "### Remarks\n\n%s\n\n", string_data(doc_parts->remarks));
    }

    if(doc_parts->see_also_count > 0) {
        fprintf(file, "### See Also\n\n");
        for(size_t i = 0; i < doc_parts->see_also_count; i++) {
            const char* name = string_data(doc_parts->see_also[i]);
            fprintf(file, "* [%s](%s.md)\n", name, name);
        }
        fprintf(file, "\n");
    }

    if(string_size(&example) != 0) {
        fprintf(file, string_data(&example));
    }

    error:
        string_free_resources(&name);
        string_free(fname);
        string_free_resources(&example);
        doc_parts_free(doc_parts);
        string_free_resources(&param_string);

        for(int i = 0; i < param_count; i++)
            string_free_resources(param_type_list + i);
        free(param_type_list);
        if(file)
            fclose(file);
}

void write_macro_doc_file(String* doc_string, String* macro, struct StringFileReadState* read_state) {
    String name = string_create("");
    String example = string_create("");
    String tags = string_create("");
    String* fname = NULL;
    DocParts* doc_parts = NULL;
    FILE* file = NULL;

    extract_macro_name(macro, &name);
    doc_parts = split_doc_parts(doc_string);

    fname = string_format(NULL, "macros/%s.md", string_data(&name));

    file = fopen(string_data(fname), "r+");

    if(file) {
        read_tags(&tags, file, read_state);
        read_example(&example, file, read_state);
        fclose(file);
        remove(string_data(fname));
    }

    file = fopen(string_data(fname), "w");
    if(!file)
        goto error;

    if(!string_is_null_or_empty(&tags))
        fprintf(file, "%s\n\n", string_data(&tags));
    
    fprintf(file, "# %s\n\n", string_data(&name));

    if(doc_parts->brief)
        fprintf(file, "%s\n\n", string_data(doc_parts->brief));

    fprintf(file, "## Syntax\n\n```c\n%s\n```\n\n", string_data(macro));

    if(doc_parts->params_count > 0) {
        fprintf(file, "| Name | Description |\n");
        fprintf(file, "| --- | --- |\n");
        String param_name = string_create("");
        String description = string_create("");
        for(int i = 0; i < doc_parts->params_count; i++) {
            string_clear(&param_name);
            string_clear(&description);

            // Remove @param from the string.
            string_erase(doc_parts->params[i], 0, 7);

            size_t name_end = string_find(doc_parts->params[i], 0, " ");
            string_substring(doc_parts->params[i], 0, name_end, &param_name);
            string_substring(doc_parts->params[i], name_end + 1, string_size(doc_parts->params[i]) - name_end - 1, &description);

            fprintf(file, "| %s | %s |\n", string_data(&param_name), string_data(&description));
        }

        fprintf(file, "\n");
    }

    if(doc_parts->ret) {
        fprintf(file, "**Returns:** %s\n\n", string_data(doc_parts->ret));
    }

    if(doc_parts->remarks) {
        fprintf(file, "### Remarks\n\n%s\n\n", string_data(doc_parts->remarks));
    }

    if(doc_parts->see_also_count > 0) {
        fprintf(file, "### See Also\n\n");
        for(size_t i = 0; i < doc_parts->see_also_count; i++) {
            const char* name = string_data(doc_parts->see_also[i]);
            fprintf(file, "* [%s](%s.md)\n", name, name);
        }
        fprintf(file, "\n");
    }

    if(string_size(&example) != 0) {
        fprintf(file, string_data(&example));
    }

    error:
        string_free_resources(&name);
        string_free(fname);
        string_free_resources(&example);
        doc_parts_free(doc_parts);

        if(file)
            fclose(file);
}

int main(void) {
    FILE* file = fopen(HEADER_LOCATION, "r");
    String doc_string = string_create("");
    String prototype = string_create("");
    char buffer[256];
    struct StringFileReadState read_state;
    string_file_read_state_init(&read_state, buffer, sizeof(buffer));

    while(read_next_docstring(&doc_string, file, &read_state)) {
        Declaration declaration_type;
        if(!read_declaration(&prototype, &declaration_type, file, &read_state))
            continue;

        switch(declaration_type) {
            case DECLARATION_FUNCTION_PROTOTYPE:
                write_function_doc_file(&doc_string, &prototype, &read_state);
                break;
            case DECLARATION_MACRO:
                write_macro_doc_file(&doc_string, &prototype, &read_state);
                break;
        }

    }

    string_free_resources(&doc_string);
    string_free_resources(&prototype);

    return EXIT_SUCCESS;
}