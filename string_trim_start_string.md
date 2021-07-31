tags: modify

# string_trim_start_string

Removes all occurences of a value from the beginning a string.

## Syntax

```c
void string_trim_start_string(String* str, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to remove the value from. |
| value | const | The string value to remove. |

## Example

```c
String str = string_create("aabbccbbaa");
String a = string_create("a");

puts(string_data(&str)); // aabbccbbaa

string_trim_string(&str, &a);

puts(string_data(&str)); // bbccbbaa

string_free_resources(&str);
string_free_resources(&a);
```