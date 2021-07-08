# string_u8_is_null_or_whitespace

Working with ASCII or UTF-8 strings, determines if the string is NULL, empty, or comprised of only whitespace characters.

## Syntax

```c
bool string_u8_is_null_or_whitespace(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to check is NULL, empty, or comprised entirely of whitespace. |

**Returns:** true if the string is NULL, empty, or comprised only of whitespace characters; false otherwise.

