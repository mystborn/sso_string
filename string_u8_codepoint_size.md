tags: unicode

# string_u8_codepoint_size

Gets the unicode character size at the specified byte index in bytes.

## Syntax

```c
int string_u8_codepoint_size(const String* str, size_t index);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to gert the unicode character size from. |
| index | size_t | The starting byte index of the unicode character. |

**Returns:** The number of bytes used to represent the unicode character.

## Example

```c
String str = string_create("こんにちは"); // Hello in Japanese.

// Gets the size of に which starts at the 6th byte index.
int size = string_u8_codepoint_size(&str, 6);

printf("Size: %d\n", size); // Size: 3

string_free_resources(&str);
```