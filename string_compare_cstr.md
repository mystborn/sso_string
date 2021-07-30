tags: operator

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

**Returns:** A negative value if str < value, zero if str == value, a positive value if str > value.

## Example

```c
String str = string_create("Hello");

bool result = string_compare_cstr(&str, "Hello");

puts(result ? "true" : "false"); // true

result = string_compare_cstr(&str, "A totally different string");

puts(result ? "true" : "false"); // false

string_free_resources(&str);
```