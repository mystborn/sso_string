# string_replace_string

Replaces a section of a string with the characters in another string.

## Syntax

```c
bool string_replace_string(String* str, size_t pos, size_t count, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string whose contents are to be replaced. |
| pos | size_t | The starting position of the contents to replace. |
| count | size_t | The number of characters following pos to replace. |
| value | const | The string value to replace the section with. |

**Returns:** @return true on success, false on allocation failure.

