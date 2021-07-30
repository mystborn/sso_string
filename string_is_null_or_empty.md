tags: property

# string_is_null_or_empty

Determines if the string is NULL or has no characters.

## Syntax

```c
bool string_is_null_or_empty(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to check is NULL or empty. |

**Returns:** true if the string is NULL or empty; false otherwise.

## Example

```c
String* str = NULL;
bool result = string_is_null_or_empty(str);
puts(result ? "true" : "false"); // true

str = string_create_ref("");
result = string_is_null_or_empty(str);
puts(result ? "true" : "false"); // true

string_append_cstr(str, "No longer empty");
result = string_is_null_or_empty(str);
puts(result ? "true" : "false"); // false

string_free(str);
```