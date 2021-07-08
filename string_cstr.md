# string_cstr

Gets the character data held by a string. This data should be altered carefully.

## Syntax

```c
char* string_cstr(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to get the internal representation of. |

**Returns:** The internal representation of the string as a c-string.

