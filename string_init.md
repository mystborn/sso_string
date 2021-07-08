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

