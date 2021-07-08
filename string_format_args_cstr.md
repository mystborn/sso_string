# string_format_args_cstr

Formats a string using printf format specifiers.

## Syntax

```c
String* string_format_args_cstr(String* result, const char* format, va_list argp);
```

| Name | Type | Description |
| --- | --- | --- |
| result | String* | A string that stores the result of the format operation. If this is NULL, this function allocates a string for the return value. Otherwise it appends the formatted data to the end. |
| format | const | A c-string that contains the text and format specifiers to be written. |
| argp | va_list | A list of the variadic arguments originally passed to a variadic function. The position of the argument may be changed, so pass a copy to this method if that isn't desired. |

**Returns:** @return result if the argument was non-null. Otherwise a newly allocated string that contains the format result. NULL on error.

