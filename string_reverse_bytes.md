tags: modify

# string_reverse_bytes

Reverses the bytes in-place in a string.

## Syntax

```c
void string_reverse_bytes(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to reverse. |

## Example

```c
String str = string_create("Hello");

puts(string_data(&str)); // Hello

string_reverse_bytes(&str);

puts(string_data(&str)); // olleH

string_free_resources(&str);
```