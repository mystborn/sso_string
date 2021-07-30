tags: memory

# string_free

Frees any resources used by a string, then frees the string itself.

## Syntax

```c
void string_free(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to clean up. |

## Example

```c
String* str = string_create_ref("I'm on the heap");

// Use the string...

string_free(str);
```