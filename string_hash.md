tags: other

# string_hash

Creates a hash code from a string using the fnv1-a algorithm.

## Syntax

```c
size_t string_hash(String* str);
```

| Name | Type | Description |
| --- | --- | --- |
| str | String* | The string to generate a hash for. |

**Returns:** A hash code for the string.

## Example

```c
String str = string_create("Hello world, it is I, your master.");
size_t hash_result = string_hash(&str);

printf("%#llx", (unsigned long long)hash_result); // 0x7d8ee846 or 0xe288df2017a06dc6

string_free_resources(&str);
```