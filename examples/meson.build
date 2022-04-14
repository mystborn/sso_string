link_args = []

if c_comp.get_id() == 'msvc' and get_option('buildtype') == 'release'
    link_args += '/NODEFAULTLIB:MSVCRTD'
endif

header_location = '/DHEADER_LOCATION="' + meson.current_source_dir() + '/../include/sso_string.h"'

string_docs_parser = executable('string_docs_parser',
    'string_docs_parser.c',
    include_directories: include_files,
    c_args: [header_location],
    link_with: sso_string_shared,
    link_args: link_args
)

playground = executable('playground',
    'playground.c',
    include_directories: include_files,
    link_with: sso_string_shared,
    link_args: link_args)