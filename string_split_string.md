tags: memory

# string_split_string

Splits a string into segments based on a separator.

## Syntax

```c
String* string_split_string(    const String* str,    const String* separator,    String* results,    int results_count,    int* results_filled,    bool skip_empty,    bool init_results);
```

| Name | Type | Description |
| --- | --- | --- |
| str |  | The string to split apart. |
| separator |  | The string to split on. |
| results |  | A contiguous array of strings that will store the split segments. Should be NULL if results_count is a negative number. |
| results_count |  | The number of elements available in the results array. If this is a negative number, the return value will be allocated by the function. |
| results_filled |  | A pointer that will contain the number of string segments added to the results array. |
| skip_empty |  | Determines if empty elements should be skipped or added to the results array. |
| init_results |  | Determines if the segment strings need to be initialized by this function. |

**Returns:** The value passed to results if results_count is a positive number. Otherwise, it's an array created by this function that will need to be manually freed. Either way, an array containing the split string segments. Returns NULL on error.

## Example

### Function Allocates

```c
String str = string_create("apples, oranges, bananas");
String separator = string_create(", ");
int count;

String* results = string_split_string(&str, &separator, NULL, STRING_SPLIT_ALLOCATE, &count, true, true);

// We can free the string that the values came from before using them
// without worry.
string_free_resources(&str);
string_free_resources(&separator);

for(int i = 0; i < count; i++) {
    puts(string_data(results + i));
}

// The values can be freed using string_split_free, but for examples case
// this is how to manually free the strings.

for(int i = 0; i < count; i++) {
    string_free_resources(results + i);
}

sso_string_free(results);

// Output:
// apples
// oranges
// bananas
```

### Caller Allocates

```c
String str = string_create("apples, oranges, bananas");
String separator = string_create(", ");
String results[2];
int count;

// The results aren't initialized, so the last argument tells the function
// to initialize them before setting them.
string_split_string(&str, &separator, results, 2, &count, true, true);

string_free_resources(&str)
string_free_resources(&separator);

// Because the results array can only fit 2 results, the last string
// (banana in this case) is ignored.

for(int i = 0; i < count; i++) {
    puts(string_data(results + i));
}

string_free_resources(results);
string_free_resources(results + 1);

// Output:
// apples
// oranges
```