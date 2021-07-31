tags: modify

# string_trim_any_start_cstr

Removes all occurrences of any value in an array from the beginning of a string.

## Syntax

```c
void string_trim_any_start_cstr(String* str, char** values, size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the values from. |
| values | char** | An array of c-strings to remove from str. |
| value_count | size_t | The number of items in values. |

## Example

```c
String str = string_create("aabbccbbaa");
char* values[2] = { "a", "b" }

puts(string_data(&str)); // aabbccbbaa

string_trim_any_start_cstr(&str, values, 2);

puts(string_data(&str)); // ccbbaa

string_free_resources(&str);
```