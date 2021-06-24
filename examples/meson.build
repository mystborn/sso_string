link_args = []

if c_comp.get_id() == 'msvc' and get_option('buildtype') == 'release'
    link_args += '/NODEFAULTLIB:MSVCRTD'
endif

string_tests = executable('string_docs_parser',
    'string_docs_parser.c',
    include_directories: include_files,
    link_with: sso_string_shared,
    link_args: link_args
)