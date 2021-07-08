# string_init_size

Initializes a string from a subsection of a c-string.

## Syntax

```c
bool string_init_size(String* str, const char* cstr, size_t length);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | A pointer to the string to initialize. |
| cstr | const | The contents to initialize the string with. |
| len | size_t | The number of characters to copy into str. |

**Returns:** @return true on success, false on allocation failure.

