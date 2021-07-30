tags: memory

# string_free_resources

Frees any resources used by a string, but does not free the string itself.

## Syntax

```c
void string_free_resources(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to clean up. |

## Example

```c
String str = string_create("Hello, stack!");

// Use the string...

// The string variable is on the stack, but it might have internal
// resources that need to be cleaned up.
string_free_resources(&str);

// OR

String* str = string_create_ref("Hello, heap!");

// Use the string...

string_free_resources(str);

// Reuse the String* without having to allocate another.
string_init(str, "Reusing the string!");

// Eventually...
string_free(str);
```