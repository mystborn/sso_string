# string_trim_end_cstr

Removes all occurences of a value from the end of a string.

## Syntax

```c
void string_trim_end_cstr(String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the value from. |
| value | const | The c-string value to remove. |

## Example

```c
String str = string_create("aabbccbbaa");

puts(string_data(&str)); // aabbccbbaa

string_trim_end_cstr(&str, "a");

puts(string_data(&str)); // aabbccbb

string_free_resources(&str);
```