# string_file_read_all

Reads the entirety of a file from its current position into a string.

## Syntax

```c
bool string_file_read_all(String* str, FILE* file);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string that will contain the file contents. |
| file | FILE* | The file to read the contents of. |

**Returns:** @return true on success, false on an error. If ferror doesn't indicate an error, it was an allocation error.

