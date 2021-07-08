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

**Returns:** @return true on success, false on allocation failure.

