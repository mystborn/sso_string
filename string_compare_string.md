# string_compare_string

Compares two strings in the same fashion as strcmp.

## Syntax

```c
int string_compare_string(const String* str, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string on the left side of the operation. |
| value | const | The string on the right side of the operation. |

**Returns:** A negative value if str < value, zero if str == value, a positive value if str > value.

