tags: file, modify

# string_file_read_line

Reads a single line of a file into a string.

## Syntax

```c
bool string_file_read_line(String* str, FILE* file);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string that will contain the line. Its contents will be overwritten. |
| file | FILE* | The file to read a line from. |

**Returns:** true if the operation was a success and there is more data to read. false if there is no more data or if there is an error. Check if there is an error using feof/ferror. If neither are set, there was an allocation error.

## Example

```c
// Assume file.txt has the following text:
/*
This is a file.
This is a new line.
Goodbye.
*/

String str = string_create("");
FILE* file = fopen("file.txt", "r");

while(string_file_read_line(&str, file, true)) {
    puts(string_data(&str));
}

fclose(file);

string_free_resources(&str);

// Output:
//
// This is a file.
// This is a new line.
// Goodbye.
```