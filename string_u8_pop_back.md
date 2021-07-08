# string_u8_pop_back

Removes a unicode character from the end of a string and returns the characters value.

## Syntax

```c
Char32 string_u8_pop_back(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to get the last unicode character of. |

**Returns:** @return The last unicode character of the string if any, '\\0' otherwise.

