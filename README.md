# memleak

CMake files are configured for Homebrew LLVM on macOS.

Memory leak test using [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html)

```zsh
% cat memory-leak.c
#include <stdlib.h>
void *p;
int main() {
  p = malloc(7);
  p = 0; // The memory is leaked here.
  return 0;
}
% clang -fsanitize=address -g memory-leak.c ; ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=lsan.supp ./a.out

=================================================================
==37858==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 7 byte(s) in 1 object(s) allocated from:
    #0 0x100edb678 in wrap_malloc+0x8c (libclang_rt.asan_osx_dynamic.dylib:arm64+0x3f678)
    #1 0x100ad7ee4 in main memory-leak.c:5
    #2 0x100af1084 in start+0x200 (dyld:arm64+0x5084)
    #3 0xbf33fffffffffffc  (<unknown module>)

-----------------------------------------------------
Suppressions used:
  count      bytes template
    127       4064 realizeClassWithoutSwift
-----------------------------------------------------

SUMMARY: AddressSanitizer: 7 byte(s) leaked in 1 allocation(s).
```

Undefined behavior test using [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html).

```zsh
% cat ub-test.c
int main(int argc, char **argv)
{
    int k = 0x7fffffff;
    k += argc;
    return 0;
}
% clang -fsanitize=undefined -g -fno-omit-frame-pointer ub-test.c ; UBSAN_OPTIONS=print_stacktrace=1 ./a.out
ub-test.c:4:7: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
    #0 0x10082ff70 in main ub-test.c:4
    #1 0x100b99088 in start+0x204 (dyld:arm64+0x5088) (BuildId: 75627683a78032adae34cf86dd23a26b32000000200000000100000000050c00)
    #2 0xdc107ffffffffffc  (<unknown module>)

SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior ub-test.c:4:7 in
```
