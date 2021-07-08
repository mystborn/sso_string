# string_resize

Resizes a string, adding the specified character to fill any new spots. Removes trailing characters if the new size is smaller than the current size.

## Syntax

```c
bool string_resize(String* str, size_t count, char ch);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to resize. |
| count | size_t | The new size of the string. |
| ch | char | The character to fill anyh new spots with. |

**Returns:** @return true on success, false on allocation failure.

