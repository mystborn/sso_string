tags: memory

# string_init

Initializes a string from a c-string.

## Syntax

```c
bool string_init(String* str, const char* cstr);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | A pointer to the string to initialize. |
| cstr | const | The contents to initialize the string with. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str;
string_init(&str, "The string value");

puts(string_data(&str)); // The string value

string_free_resources(&str);
```