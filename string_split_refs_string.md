# string_split_refs_string

Splits a string into segments based on a separator.

## Syntax

```c
String** string_split_refs_string(    const String* str,    const String* separator,    String** results,    int results_count,    int* results_filled,    bool skip_empty,    bool allocate_results);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string to split apart. |
| separator |  | The string to split on. |
| results |  | A non-contiguous array of strings that will store the split segments. Should be NULL if results_count is a negative number. |
| results_count |  | The number of elements available in the results array. If this is a negative number, the return value will be allocated by the function. |
| results_filled |  | A pointer that will contain the number of string segments added to the results array. |
| skip_empty |  | Determines if empty elements should be skipped or added to the results array. |
| init_results |  | Determines if the segment strings need to be allocated by this function. If this is false, the results will need to be allocated/initialized beforehand. |

**Returns:** The value passed to results if results_count is a positive number. Otherwise, it's an array created by this function that will need to be manually freed. Either way, an array containing the split string segments. Returns NULL on error.

