# string_trim_any_cstr

Removes all occurrences of any value in an array from the beginning and end of a string.

## Syntax

```c
void string_trim_any_cstr(String* str, char** values, size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the values from. |
| values | char** | An array of c-strings to remove from str. |
| value_count | size_t | The number of items in values. |

