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

**Returns:** @return true on success, false on allocation failure.

