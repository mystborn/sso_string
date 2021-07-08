# string_u8_push_back

Appends a unicode character to the end of a string.

## Syntax

```c
bool string_u8_push_back(String* str, Char32 value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to append to. |
| value | Char32 | The unicode character to append. |

**Returns:** @return true on success, false on allocation failure.

