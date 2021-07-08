# string_append_string_part

Appends a section of a string to the end of another string.

## Syntax

```c
bool string_append_string_part(    String* str,     const String* value,     size_t start,     size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string to append to. |
| value |  | The string to append. |
| start |  | The starting index of the section of the value to append. |
| count |  | The number of characters following start to append. |

**Returns:** true on success, false on allocation failure.

