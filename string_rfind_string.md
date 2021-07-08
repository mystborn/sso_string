# string_rfind_string

Finds the starting index of the last occurrence of a string in another string.

## Syntax

```c
size_t string_rfind_string(const String* str, size_t pos, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to search. |
| pos | size_t | The starting position in the string to start searching, starting from the back. |
| value | const | The string value to search for. |

**Returns:** The starting index of the substring on success, or SIZE_MAX if the substring couldn't be found.

