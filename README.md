# memleak
Memory leak test using [LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html)

Added leak suppression for `realizeClassWithoutSwift` on macOS.

```
$ cat memory-leak.c
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
