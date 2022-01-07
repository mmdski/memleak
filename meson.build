project('memleak', 'c',
    default_options : ['c_std=c99',
        'c_args=-Wall',
        'b_ndebug=if-release',
        'b_sanitize=address',
        'b_lundef=false'],
    version : '0.0.0')

mem_test = executable('memleak_test',
    'memory-leak.c')

test_env = environment()
test_env.set('ASAN_OPTIONS', 'detect_leaks=1')
test('testmem', mem_test, env: test_env)
