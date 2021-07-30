tags: property

# string_data

Gets the character data held by a string. This data cannot be altered.

## Syntax

```c
const char* string_data(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the internal representation of. |

**Returns:** The internal representation of the string as a c-string.

## Example

```c
String str = string_create("Hello, data");

const char* data = string_data(&str);
puts(data); // Hello, data

string_free_resources(&str);
```