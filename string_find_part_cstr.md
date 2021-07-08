# string_find_part_cstr

Finds the starting index of the first occurrence of part of a c-string in a string.

## Syntax

```c
size_t string_find_part_cstr(const String* str, size_t pos, const char* value, size_t start, size_t length);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to search. |
| pos | size_t | The starting position in the string to start searching. |
| value | const | The c-string value to search for. |
| start | size_t | The beginning index of value to search for in the string. |
| length | size_t | The number of characters following start in value to search for in the string. |

**Returns:** @return The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.

