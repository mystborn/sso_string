tags: compare

# string_compare_string

Compares two strings in the same fashion as strcmp.

## Syntax

```c
int string_compare_string(const String* str, const String* value);
```

| Name | Type | Description |
| --- | --- | --- |
| str | const | The string on the left side of the operation. |
| value | const | The string on the right side of the operation. |

**Returns:** A negative value if str < value, zero if str == value, a positive value if str > value.

## Example

```c
String str = string_create("Hello");
String hello = string_create("Hello");
String other = string_create("A totally different string");

int result = string_compare_string(&str, &hello);

printf("%d\n", result); // 0

result = string_compare_cstr(&str, &other);

printf("%d\n", result); // -1

string_free_resources(&str);
string_free_resources(&hello);
string_free_resources(&other);
```