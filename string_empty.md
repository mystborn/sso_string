tags: property

# string_empty

Determines if a string has no characters.

## Syntax

```c
bool string_empty(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to check is empty. |

**Returns:** true if the string is empty; false otherwise.

## Example

```c
String str = string_create("");

bool empty = string_empty(&str);
puts(empty ? "true" : "false"); // true

string_append_cstr(&str, "Not empty anymore");
empty = string_empty(&str);
puts(empty ? "true" : "false"); // false

string_free_resources(&str);
```