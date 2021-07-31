tags: modify

# string_trim_any_start_string_refs

Removes all occurrences of any value in an array from the beginning of a string.

## Syntax

```c
void string_trim_any_start_string_refs(String* str, String** values, size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the values from. |
| values | String** | An array of string references to remove from str. |
| value_count | size_t | The number of items in values. |

## Example

```c
String str = string_create("aabbccbbaa");
String values[2] = { string_create_ref("a"), string_create_ref("b") };

puts(string_data(&str)); // aabbccbbaa

string_trim_any_start_string_refs(&str, values, 2);

puts(string_data(&str)); // ccbbaa

string_free_resources(&str);

string_free(values);
string_free(values + 1);
```