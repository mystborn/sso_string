tags: memory, modify

# string_resize

Resizes a string, adding the specified character to fill any new spots. Removes trailing characters if the new size is smaller than the current size.

## Syntax

```c
bool string_resize(String* str, size_t count, char ch);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to resize. |
| count | size_t | The new size of the string. |
| ch | char | The character to fill anyh new spots with. |

**Returns:** true on success, false on allocation failure.

## Example

```c
// Grow Example

String grow = string_create("Grow");

puts(string_data(&grow)); // Grow

string_resize(&grow, 7, '0');

puts(string_data(&grow)); // Grow000

string_free_resources(&grow);

// Shrink Example

String shrink = string_create("Shrink");

puts(string_data(&shrink)); // Shrink

string_resize(&shrink, 4, '0');

puts(string_data(&shrink)); // Shri

string_free_resources(&shrink);
```