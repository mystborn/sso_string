# string_trim_any_string_refs

Removes all occurrences of any value in an array from the beginning and end of a string.

## Syntax

```c
void string_trim_any_string_refs(String* str, String** values, size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the values from. |
| values | String** | An array of string referencess to remove from str. |
| value_count | size_t | The number of items in values. |

