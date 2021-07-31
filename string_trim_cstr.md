tags: modify

# string_trim_cstr

Removes all occurences of a value from the beginning and end of a string.

## Syntax

```c
void string_trim_cstr(String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the value from. |
| value | const | The c-string value to remove. |

## Example

```c
String str = string_create("aabbccbbaa");

puts(string_data(&str)); // aabbccbbaa

string_trim_cstr(&str, "a");

puts(string_data(&str)); // bbccbb

string_free_resources(&str);
```