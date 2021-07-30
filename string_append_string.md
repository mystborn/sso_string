tags: modify

# string_append_string

Appends a string to the end of another string.

## Syntax

```c
bool string_append_string(String* str, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to append to. |
| value | const | The string to append. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("Hello");
String world = string_create(" world");

puts(string_data(&str)); // Hello

string_append_string(&str, &world);

puts(string_data(&str)); // Hello world

string_free_resources(&str);
```