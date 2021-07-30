tags: modify

# string_insert_part_cstr

Inserts a section of a c-string into a string at the specified index.

## Syntax

```c
bool string_insert_part_cstr(String* str, const char* value, size_t index, size_t start, size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to insert into. |
| value | const | The c-string to insert. |
| index | size_t | The index of the string to insert the value into. |
| start | size_t | The starting index of the value to insert. |
| count | size_t | The number of characters following start to insert. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("Hello.");
char* cstr = "The world is large";

puts(string_data(&str)); // Hello.

// Insert " world" before the period in str.
string_insert_part_cstr(&str, cstr, 5, 3, 6);

puts(string_data(&str)); // Hello world.

string_free_resources(&str);
```