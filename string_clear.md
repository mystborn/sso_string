tags: modify

# string_clear

Clears the contents of a string, but does not free any allocated memory.

## Syntax

```c
void string_clear(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to clear. |

### See Also

* [string_shrink_to_fit](string_shrink_to_fit.md)

```c
String str = string_create("I don't realy want this text.");

puts(string_data(&str)); // I don't realy want this text.

string_clear(&str);

// The string is empty, so just adds a newline from the puts call.
puts(string_data(&str));

string_free_resources(&str);
```