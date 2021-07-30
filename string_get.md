tags: index, property

# string_get

Gets the character at the specified index in a string.

## Syntax

```c
char string_get(const String* str, size_t index);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the character from. |
| index | size_t | The index of the character to get. |

**Returns:** The character at the specified index.

```c
String str = string_create("Get");

char letter = string_get(&str, 1);
printf("%c\n", letter); // e

string_free_resources(&str);
```