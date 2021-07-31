tags: modify

# string_trim_any_string

Removes all occurrences of any value in an array from the beginning and end of a string.

## Syntax

```c
void string_trim_any_string(String* str, String* values, size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the values from. |
| values | String* | An array of strings to remove from str. |
| value_count | size_t | The number of items in values. |

## Example

```c
String str = string_create("aabbccbbaa");
String values[2] = { string_create("a"), string_create("b") };

puts(string_data(&str)); // aabbccbbaa

string_trim_any_string(&str, values, 2);

puts(string_data(&str)); // cc

string_free_resources(&str);
```