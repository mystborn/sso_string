# string_compare_cstr

Compares a string and a c-string in the same fashion as strcmp.

## Syntax

```c
int string_compare_cstr(const String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string on the left side of the operation. |
| value | const | The c-string on the right side of the operation. |

**Returns:** @return A negative value if str < value, zero if str == value, a positive value if str > value.

