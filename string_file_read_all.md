tags: file, modify

# string_file_read_all

Reads the entirety of a file from its current position into a string.

## Syntax

```c
bool string_file_read_all(String* str, FILE* file);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string that will contain the file contents. |
| file | FILE* | The file to read the contents of. |

**Returns:** true on success, false on an error. If ferror doesn't indicate an error, it was an allocation error.

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

string_file_read_all(str, file);

fclose(file);

puts(string_data(&str));

string_free_resources(&str);

// Output:
// 
// This is a file.
// This is a new line.
// Goodbye.
```