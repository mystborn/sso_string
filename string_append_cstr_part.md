# string_append_cstr_part

Appends a section of a c-string to the end of a string.

## Syntax

```c
bool string_append_cstr_part(    String* str,     const char* value,     size_t start,     size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string to append to. |
| value |  | The c-string to append. |
| start |  | The starting index of the section of the value to append. |
| count |  | The number of characters following start to append. |

**Returns:** true on success, false on allocation failure.

