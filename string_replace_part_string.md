# string_replace_part_string

Replaces a section of a string with the characters in another string slice.

## Syntax

```c
bool string_replace_part_string(String* str, size_t pos, size_t count, const String* value, size_t start, size_t length);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string whose contents are to be replaced. |
| pos | size_t | The starting position of the contents to replace. |
| count | size_t | The number of characters following pos to replace. |
| value | const | The string value to replace the section with. |
| start | size_t | The starting position of value to use as a replacement. |
| length | size_t | The number of characters following start in value to use as a replacement. |

**Returns:** true on success, false on allocation failure.

