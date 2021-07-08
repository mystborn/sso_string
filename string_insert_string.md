# string_insert_string

Inserts a string into another string at the specified index.

## Syntax

```c
bool string_insert_string(String* str, const String* value, size_t index);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to insert into. |
| value | const | The string to insert. |
| index | size_t | The index of the string to insert the value into. |

**Returns:** true on success, false on allocation failure.

