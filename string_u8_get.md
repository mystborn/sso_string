tags: index, unicode

# string_u8_get

Gets the unicode character at the specified byte index.

## Syntax

```c
Char32 string_u8_get(const String* str, size_t index);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to get the unicode character from. |
| index | size_t | The starting byte index of the unicode character. |

**Returns:** Thee unicode character starting at the specified byte index.

## Example

```c
String str = string_create("こんにちは"); // Hello in Japanese.

Char32 letter = string_u8_get(&str, 3);

putwc(letter, stdout); // ん

string_free_resources(&str);
```