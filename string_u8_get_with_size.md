tags: index, unicode

# string_u8_get_with_size

Gets the unicode character at the specified byte index, optionally getting the number of bytes that the character takes in the string.

## Syntax

```c
Char32 string_u8_get_with_size(const String* str, size_t index, int* out_size);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the unicode character from. |
| index | size_t | The starting byte index of the unicode character. |
| out_size | int* | If not NULL, contains the number of bytes used to represent unicode character. |

**Returns:** Thee unicode character starting at the specified byte index. @remark This function can be used to easily iterate over a UTF-8 string.

## Example

```c
String str = string_create("こんにちは"); // Hello in Japanese.

int size;
Char32 codepoint = string_u8_get_with_size(&str, 3, &size);

putwc(codepoint, stdout); // ん
printf("Character size in bytes: %d\n"); // Character size in bytes: 3

// This function is useful for iterating over UTF-8 strings like so:

int size = 0;
int index = 0;
for(; index < string_size(&str); index += size) {
    Char32 codepoint = string_u8_get_with_size(&str, index, &size);
    // Use the codepoint somehow...
}

string_free_resources(&str);
```