tags: compare

# string_equals_cstr

Determines if the contents of a String is equivalent to a c-string.

## Syntax

```c
bool string_equals_cstr(const String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string on the left side of the operation. |
| value | const | The c-string on the right side of the operation. |

**Returns:** true if the values are equivalent; false otherwise.

## Example

```c
String str = string_create("Hello");

bool result = string_equals_cstr(&str, "Hello");

puts(result ? "true" : "false"); // true

result = string_equals_cstr(&str, "A totally different string");

puts(result ? "true" : "false"); // false

string_free_resources(&str);
```