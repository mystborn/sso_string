tags: compare

# string_starts_with_cstr

Determines if a string starts with the characters in a c-string.

## Syntax

```c
bool string_starts_with_cstr(const String* str, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to check the beginning of. |
| value | const | The value to check the beginning of the string for. |

**Returns:** true if the string starts with the value; false otherwise.

## Example

```c
String str = string_create("Hello");

bool result = string_starts_with_cstr(&str, "He");

puts(result ? "true" : "false"); // true

result = string_starts_with_cstr(&str, "Hi");

puts(result ? "true" : "false"); // false

string_free_resource(&str);
```