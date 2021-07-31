tags: compare

# string_equals_string

Determines if the contents of two strings are equivalent.

## Syntax

```c
bool string_equals_string(const String* str, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string on the left side of the operation. |
| value | const | The string on the right side of the operation. |

**Returns:** true if the values are equivalent; false otherwise.

## Example

```c
String str = string_create("Hello");
String hello = string_create("Hello");
String other = string_create("A totally different string");

bool result = string_compare_string(&str, &hello);

puts(result ? "true" : "false"); // true

result = string_compare_cstr(&str, &other);

puts(result ? "true" : "false"); // false

string_free_resources(&str);
string_free_resources(&hello);
string_free_resources(&other);
```