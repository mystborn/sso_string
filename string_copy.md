tags: memory

# string_copy

Initializes a string with the data of another string.

## Syntax

```c
bool string_copy(const String* str, String* out_value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to copy. |
| out_value | String* | The string to copy the contents into. This value should not be initialized by the caller, or it might cause a memory leak. |

**Returns:** true on success, false on allocation failure.

## Example

```c
Stirng str = string_create("Copy");
String out_value;

string_copy(&str, &out_value);

puts(string_data(&out_value)); // Copy
```