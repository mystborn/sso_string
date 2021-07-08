# Version 0.2.0

### API Additions
* `string_trim`, `string_trim_start`, and `string_trim_end` family.
    * Generic macros available for each.
* `string_pad` and `string_u8_pad` family.
    * Generic macros available for each.
* `string_replace_part` family.
    * Generic macros available for each.
* `string_file_read_line`
* `string_file_read_all`
* Optionally remove generic macros by defining `SSO_STRING_NO_GENERIC`.
* Added support for custom allocation functions.
    * Define `sso_string_malloc`, `sso_string_realloc`, and `sso_string_free` to the custom functions and they will be used instead of the standard library versions of `malloc`, `realloc`, and `free`.
    * These can be global function pointers as well as long as they're initialized before the string library is used at all.

### API Changes
* `string_split` and `string_join` changed to have `_string`/`_cstr` suffixes.
    * Generic versions were added that use the original names.
* `string_find_substr` family changed to `string_find_part`.
    * This change was made to better match the rest of the API.
* `string_insert_cstr_part` and `string_insert_string_part` changed to `string_insert_part_cstr` and `string_insert_part_string` respectively.
    * This change was made to better match the rest of the API.

### Bug Fixes

### Others
* Added an examples folder.
* Added an example that is actually used to generate the site documentation.

# Version 0.1.0

Initial Release!