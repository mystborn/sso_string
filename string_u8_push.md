tags: unicode, modify

# string_u8_push

Appends a unicode character to the end of a string.

## Syntax

```c
bool string_u8_push(String* str, Char32 value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to append to. |
| value | Char32 | The unicode character to append. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("こんにちは"); // Hello in Japanese.

Char32 period = L'。'; // A Japanese period.

puts(string_data(&str)); // こんにちは

string_u8_push(&str, period);

puts(string_data(&str)); //こんにちは。

string_free_resources(&str);
```