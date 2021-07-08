# string_join_refs

Joins an array of strings together into a single string with a common separator in between each of them.

## Syntax

```c
bool string_join_refs(    String* str,    const String* separator,    const String** values,    size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string that stores the result. If it has a value, the result is appended to the end. It needs to be initialized. |
| separator |  | A string to separate each array value in the result. |
| values |  | An array of string references to combine. |
| value_count |  | The number of strings in the values array. |

