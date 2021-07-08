# string_format_time_cstr

Formats a string based on a time value using strftime format specifiers.

## Syntax

```c
String* string_format_time_cstr(String* result, const char* format, const struct tm* timeptr);
```

| Name | Type | Description |
| --- | --- | --- |
| result | String* | A string that stores the result of the format operation. If this is NULL, this function allocates a string for the return value. Otherwise it appends the formatted data to the end. |
| format | const |  A string that contains the text and time format specifiers to be written. |
| timeptr | const | The time value used to format the string. |

**Returns:** @return result if the argument was non-null. Otherwise a newly allocated string that contains the time format result. NULL on error.

