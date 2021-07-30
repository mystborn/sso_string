tags: modify

# string_append_part_string

Appends a section of a string to the end of another string.

## Syntax

```c
bool string_append_part_string(    String* str,     const String* value,     size_t start,     size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string to append to. |
| value |  | The string to append. |
| start |  | The starting index of the section of the value to append. |
| count |  | The number of characters following start to append. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("Hello");
String world = string_create("The world is large");

puts(string_data(str)); // Hello

string_append_part_string(str, world, 3, 6);

puts(string_data(str)); // Hello world

string_free_resources(&str);
string_free_resources(&world);
```