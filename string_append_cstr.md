tags: modify

# string_append_cstr

Appends a c-string to the end of a string.

## Syntax

```c
bool string_append_cstr(String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to append to. |
| value | const | The c-string to append. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("Hello");

puts(string_data(&str)); // Hello

string_append_cstr(&str, " world");

puts(string_data(&str)); // Hello world

string_free_resources(&str);
```