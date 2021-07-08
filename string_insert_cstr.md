# string_insert_cstr

Inserts a c-string into a string at the specified index.

## Syntax

```c
bool string_insert_cstr(String* str, const char* value, size_t index);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to insert into. |
| value | const | The c-string to insert. |
| index | size_t | The index of the string to insert the value into. |

**Returns:** @return true on success, false on allocation failure.

