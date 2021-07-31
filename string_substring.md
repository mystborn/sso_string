tags: memory

# string_substring

Initializes a string with the data from a slice of another string slice.

## Syntax

```c
bool string_substring(const String* str, size_t pos, size_t count, String* out_value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the slice from. |
| pos | size_t | The starting position of the slice. |
| count | size_t | The number of characters following pos to copy into the substring. |
| out_value | String* | The string that will be initialized with the contents of the substring. This value should not be initialized by the caller, or it might cause a memory leak. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("Hello beautiful world");
String out_value;

string_substring(&str, 6, 9, &out_value);

puts(string_data(&out_value)); // beautiful

string_free_resources(&str);
string_free_resources(&out_value);
```