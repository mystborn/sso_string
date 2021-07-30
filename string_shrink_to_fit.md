tags: memory

# string_shrink_to_fit

Removes any excess memory not being used by a string.

## Syntax

```c
void string_shrink_to_fit(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to shrink. |

## Example

```c
String str = string_create("");

// Reserve a lot of memory for some operation.
string_reserve(&str, 10000);

// Perform the operation...

// Trim any excess memory from the end.
string_shrink_to_fit(&str);
```