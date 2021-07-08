# string_u8_codepoint_size

Gets the unicode character size at the specified byte index in bytes.

## Syntax

```c
int string_u8_codepoint_size(const String* str, size_t index);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to gert the unicode character size from. |
| index | size_t | The starting byte index of the unicode character. |

**Returns:** @return The number of bytes used to represent the unicode character.

