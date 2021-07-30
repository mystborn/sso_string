tags: lifecycle

# string_create_ref

Allocates and initializes a new string.

## Syntax

```c
String* string_create_ref(const char* cstr);
```

| Name | Type | Description |
| --- | --- | --- |
| cstr | const | The contents to initialize the string with. |

**Returns:** The initialized String reference. Must be manually freed. NULL on allocation failure.

## Example

```c
String* str = string_create_ref("The result was allocated on the heap.");

puts(string_data(str));

string_free(str);
```