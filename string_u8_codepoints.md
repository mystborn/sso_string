tags: unicode, property

# string_u8_codepoints

Gets the number of codepoints in a string.

## Syntax

```c
size_t string_u8_codepoints(const String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the codepoint count of. |

**Returns:** The number of codepoints in the string.

```c
String str = string_create("こんにちは"); // Hello in Japanese.

// Using int in example to mkae printf easier.
int size = string_size(&str);
int codepoints = string_u8_codepoints(&str);

printf("Bytes: %d\nCodepoints: %d\n", size, codepoints);
// Outputs:
// Bytes: 15
// Codepoints: 5

string_free_resources(&str);
```