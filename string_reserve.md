tags: memory

# string_reserve

Ensures that a string has a capacity large enough to hold a specified number of characters. Does not include any terminating characters.

## Syntax

```c
bool string_reserve(String* str, size_t reserve);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | - The string to potentially enlarge. |
| reserve | size_t | - The desired minimum capacity, not including the NULL terminating character. |

**Returns:** true on success, false on allocation failure.

## Example

```c
String str = string_create("");

string_reserve(&str, 512);

// This array is guarunteed to be at least 512 bytes + 1 for the NULL-terminating character.
// It can be fed into a function that expects a c-string. Just make sure to set the size of the
// string afterwards.
char* data = string_cstr(&str);

some_function_expecting_a_c_string(data);

// This makes sure the size of the string is synchronized with the size of the c-string.
sso_string_set_size(str, strlen(data));

string_free_resources(&str);
```