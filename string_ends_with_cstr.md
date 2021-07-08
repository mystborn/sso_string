# string_ends_with_cstr

Determines if a string ends with the characters in a c-string.

## Syntax

```c
bool string_ends_with_cstr(const String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to check the ending of. |
| value | const | The value to check the ending of the string for. |

**Returns:** @return true if the string ends with the value; false otherwise.
