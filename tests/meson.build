if check_location != ''
    check_lib = check_location + '/lib'
    
    compat = c_comp.find_library('compat',
        required: true,
        dirs: check_lib)

    check = c_comp.find_library('check',
        required: true,
        dirs: check_lib)

    deps = [compat, check]
else
    check = c_comp.find_library('check',
        required: true)

    deps = [check]
endif

link_args = []

if c_comp.get_id() == 'msvc' and get_option('buildtype') == 'release'
    link_args += '/NODEFAULTLIB:MSVCRTD'
endif


string_tests = executable('string_tests',
    'tests.c',
    include_directories: test_inc,
    dependencies: deps,
    link_args: link_args
)

test('String Tests', string_tests)