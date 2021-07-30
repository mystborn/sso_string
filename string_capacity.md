tags: property, memory

# string_capacity

Gets the number of characters a string can potential hold without resizing. This does NOT include the NULL terminating character.

## Syntax

```c
size_t string_capacity(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the capacity of. |

**Returns:** The internal capacity of the string.

```c
String str = string_create("small");

int size = string_capacity(&str);

printf("Capacity: %d", size);
// The output is platform dependant.
// On x64, prints:
// Capacity: 22

string_free_resources(&str);