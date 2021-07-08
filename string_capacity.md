# string_capacity

Gets the number of characters a string can potential hold without resizing. This does NOT include the NULL terminating character.

## Syntax

```c
size_t string_capacity(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the capacity of. |

**Returns:** @return The internal capacity of the string.

