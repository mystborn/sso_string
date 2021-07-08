# string_u8_set

Replaces the unicode character at the specified index in a string.

## Syntax

```c
bool string_u8_set(String* str, size_t index, Char32 value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to modify. |
| index | size_t | The index of the character to replace. |
| value | Char32 | The new character to replace the existing character with. |

**Returns:** true on success, false on allocation failure.

