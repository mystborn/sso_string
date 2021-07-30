tags: modify

# string_append_part_cstr

Appends a section of a c-string to the end of a string.

## Syntax

```c
bool string_append_part_cstr(String* str, const char* value, size_t start, size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string to append to. |
| value |  | The c-string to append. |
| start |  | The starting index of the section of the value to append. |
| count |  | The number of characters following start to append. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("Hello");
char* world = "The world is large";

puts(string_data(str)); // Hello

string_append_part_cstr(str, world, 3, 6);

puts(string_data(str)); // Hello world

string_free_resources(&str);
string_free_resources(&world);
```