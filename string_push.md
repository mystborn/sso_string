tags: modify

# string_push

Appends a character to the end of a string.

## Syntax

```c
bool string_push(String* str, char value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to append to. |
| value | char | The character to append. |

**Returns:** true on success, false on allocation failure.

## Example

```c
string_create("Hello");

puts(string_data(&str)); // Hello

string_push(&str, '.');

puts(string_data(&str)); // Hello.

string_free_resources(&str);
```