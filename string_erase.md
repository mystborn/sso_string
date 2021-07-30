tags: modify

# string_erase

Removes a section from a string.

## Syntax

```c
void string_erase(String* str, size_t index, size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to modify. |
| index | size_t | The starting index of the characters to erase. |
| count | size_t | The number of characters following index to erase. |

## Example

```c
String str = string_create("Hello world.");

puts(string_data(&str)); // Hello world.

string_erase(&str, 5, 6);

puts(string_data(&str)); // Hello.

string_free_resources(&str);
```