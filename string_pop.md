tags: modify

# string_pop

Removes a character from the end of a string and returns the characters value.

## Syntax

```c
char string_pop(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to get the last character of. |

**Returns:** The last character of the string if any, '\\0' otherwise.

## Example

```c
String str = string_create("Hello.");

puts(string_data(&str)); // Hello.

string_pop(&str);

puts(string_data(&str)); // Hello

string_free_resources(&str);
```