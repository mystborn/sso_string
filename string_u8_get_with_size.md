# string_u8_get_with_size

Gets the unicode character at the specified byte index, optionally getting the number of bytes that the character takes in the string.

## Syntax

```c
Char32 string_u8_get_with_size(const String* str, size_t index, int* out_size);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the unicode character from. |
| index | size_t | The starting byte index of the unicode character. |
| out_size | int* | If not NULL, contains the number of bytes used to represent unicode character. |

**Returns:** @return Thee unicode character starting at the specified byte index. @remark This function can be used to easily iterate over a UTF-8 string.

