tags: index, modify

# string_set

Sets the character at the specified index in a string.

## Syntax

```c
void string_set(String* str, size_t index, char value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to modify. |
| index | size_t | The index of the character to replace. |
| value | char | The new character to replace the existing character with. |

## Example

```c
String str = string_create("Hello");

puts(string_data(&str)); // Hello

string_set(&str, 2, 'n');

puts(string_data(&str)); // Henlo

string_free_resources(&str);

```