tags: modify

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

## Example

```c
String str = string_create("");
String separator = string_create(", ");

String* values[] = { string_create_ref("apples), string_create_ref("oranges"), string_create_ref("bananas") };

string_join_refs(&str, &separator, values, 3);

puts(string_data(&str)); // apples, oranges, bananas

string_free_resources(&str);
string_free_resources(&separator);
string_free(values);
string_free(values + 1);
string_free(values + 2);
```