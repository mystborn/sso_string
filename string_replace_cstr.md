tags: modify

# string_replace_cstr

Replaces a section of a string with the characters in a c-string.

## Syntax

```c
bool string_replace_cstr(String* str, size_t pos, size_t count, const char* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string whose contents are to be replaced. |
| pos | size_t | The starting position of the contents to replace. |
| count | size_t | The number of characters following pos to replace. |
| value | const | The c-string value to replace the section with. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String 
```