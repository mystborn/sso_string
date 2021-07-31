tags: unicode, modify

# string_u8_pad_left

Pads the beginning of a string with a unicode character until it's at least the specified codepoints long.

## Syntax

```c
bool string_u8_pad_left(String* str, Char32 value, size_t width);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to pad. |
| value | Char32 | The unicode character to pad the string with. |
| width | size_t | The minimum number of codepoints in the string after it's padded. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("こんにちは"); // Hello in Japanese.
Char32 period = L'。'; // A Japanese period.

puts(string_data(&str)); // こんにちは

string_u8_pad_left(&str, period, 7);

puts(string_data(&str)); // 。。こんにちは

string_free_resources(&str);
```