tags: property

# string_cstr

Gets the character data held by a string. This data should be altered carefully.

## Syntax

```c
char* string_cstr(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to get the internal representation of. |

**Returns:** The internal representation of the string as a c-string.

## Example

```
String str = string_create("hrllo");

char* cstr = string_cstr(&str);

puts(cstr); // hrllo;

// Oops, I made a mistake. modify the raw cstr.

cstr[1] = 'e';

// The internal string has been altered.
puts(string_cstr(&str)); // hello

string_free_resources(&str);
```