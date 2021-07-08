# string_copy_to

Copies the data from a slice of a string into a c-string, overwriting any previous data. Does not add a terminating character at the end.

## Syntax

```c
void string_copy_to(const String* str, char* cstr, size_t pos, size_t count);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string to copy. |
| cstr | char* | The c-string to copy into. |
| pos | size_t | The starting position of the string to start copying into the c-string. |
| count | size_t | The number of characters following pos to copy into the c-string. |
