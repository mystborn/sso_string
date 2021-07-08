# string_reserve

Ensures that a string has a capacity large enough to hold a specified number of characters. Does not include any terminating characters.

## Syntax

```c
bool string_reserve(String* str, size_t reserve);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | - The string to potentially enlarge. |
| reserve | size_t | - The desired minimum capacity, not including the NULL terminating character. |

**Returns:** true on success, false on allocation failure.

