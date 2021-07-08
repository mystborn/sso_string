# string_create_ref

Allocates and initializes a new string.

## Syntax

```c
String* string_create_ref(const char* cstr);
```

| Name | Type | Description |
| --- | --- | --- |
| cstr | const | The contents to initialize the string with. |

**Returns:** @return The initialized String reference. Must be manually freed. NULL on allocation failure.

