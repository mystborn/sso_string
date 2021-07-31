tags: unicode, modify

# string_u8_reverse_codepoints

Reverses the contents of a string in-place based on UTF-8 codepoints.

## Syntax

```c
void string_u8_reverse_codepoints(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to reverse. |

## Example

```c
String str = string_create("こんにちは"); // Hello in Japanese.

puts(string_data(&str)); // こんにちは

string_u8_reverse_codepoints(&str);

puts(string_data(&str)); // はちにんこ

string_free_resources(&str);
```