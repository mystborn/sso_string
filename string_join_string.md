tags: modify

# string_join_string

Joins an array of strings together into a single string with a common separator in between each of them.

## Syntax

```c
bool string_join_string(    String* str,     const String* separator,    const String* values,    size_t value_count);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string that stores the result. If it has a value, the result is appended to the end. It needs to be initialized. |
| separator |  |  A string to separate each array value in the result. |
| values |  |  An array of strings to combine. |
| value_count |  |  The number of strings in the values array. |

## Example

```c
String str = string_create("");
String separator = string_create(", ");

String values[] = { string_create("apples), string_create("oranges"), string_create("bananas") };

string_join_string(&str, &separator, values, 3);

puts(string_data(&str)); // apples, oranges, bananas

string_free_resources(&str);
string_free_resources(&separator);
string_free_resources(values);
string_free_resources(values + 1);
string_free_resources(values + 2);
```