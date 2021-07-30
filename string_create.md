tags: lifecycle

# string_create

Creates and initializes a new string value.

## Syntax

```c
String string_create(const char* cstr);
```

| Name | Type | Description |
| --- | --- | --- |
| cstr | const | The contents to initialize the string with. |

**Returns:** The initialized String value.

## Example

```c
String str = string_create("Easy way to create a string.");

puts(string_data(&str)); // Easy way to create a string.

string_free_resources(&str);
```