# string_pad_right

Pads the end of a string with a character until it's at least the specified size.

## Syntax

```c
bool string_pad_right(String* str, char value, size_t width);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to pad. |
| value | char | The character to pad the string with. |
| width | size_t | The minimum size of the string after it's padded. |

**Returns:** true on success, false on allocation failure.

