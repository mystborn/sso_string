tags: compare

# string_starts_with_string

Determines if a string starts with the characters in another string.

## Syntax

```c
bool string_starts_with_string(const String* str, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to check the beginning of. |
| value | const | The value to check the beginning of the string for. |

**Returns:** true if the string starts with the value; false otherwise.

## Example

```c
String str = string_create("Hello");
String he = string_create("He");
String hi = string_create("Hi);

bool result = string_starts_with_string(&str, &he);

puts(result ? "true" : "false"); // true

result = string_starts_with_string(&str, &hi);

puts(result ? "true" : "false"); // false

string_free_resource(&str);
```