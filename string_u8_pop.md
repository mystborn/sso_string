tags: unicode, modify

# string_u8_pop

Removes a unicode character from the end of a string and returns the characters value.

## Syntax

```c
Char32 string_u8_pop(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to get the last unicode character of. |

**Returns:** The last unicode character of the string if any, '\\0' otherwise.

## Example

```c
String str = string_create("こんにちは。") // Hello in Japanese

puts(string_data(&str)); // こんにちは。

string_u8_pop(&str);

puts(string_data(&str)); // こんにちは

string_free_resources(&str);
```