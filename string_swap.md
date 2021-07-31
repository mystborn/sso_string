tags: memory

# string_swap

Swaps the contents of two strings.

## Syntax

```c
void string_swap(String* left, String* right);
```

| Name | Type | Description |
| --- | --- | --- |
| left | String* | The string on the left side of the operation. |
| right | String* | The string on the right side of the operation. |

## Example

```c
String left = string_create("Hello");
String right = string_create("World");

printf("Left: %s, Right: %s", string_data(&left), string_data(&right));
// Left: Hello, Right: World

string_swap(&left, &right);

printf("Left: %s, Right: %s", string_data(&left), string_data(&right));
// Left: World, Right: Hello
```