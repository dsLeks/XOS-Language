# XOS-Language

XOS? SAUCE?

![Build Status](https://github.com/dsLeks/XOS-Language/actions/workflows/build.yml/badge.svg)
![Build Status Sanitized](https://github.com/dsLeks/XOS-Language/actions/workflows/build-sanitized.yml/badge.svg)

## Min Requirements

This could potentially build with earlier versions. These just happen to be the
ones tested with locally.

- gcc v13
  - Can also use clang v16
- make v4
- [googletest v1.12.1](https://github.com/google/googletest/releases/tag/release-1.12.1)  (only needed for testing)
- [LLVM v16](https://github.com/llvm/llvm-project/releases/tag/llvmorg-16.0.0)
  - NOTE: If using a prebuilt for x86_64 linux, use the
    [16.04](https://github.com/llvm/llvm-project/releases/tag/llvmorg-16.0.4)
    prebuilt. The normal v16.0.0 version seems to be built incorrectly since I
    run into a segfault with the prebuilt in
    `llvm::ConstantExpr::getGetElementPtr(llvm::Type*, llvm::Constant*,
    llvm::ArrayRef<llvm::Value*>, bool, std::optional<unsigned int>,
    llvm::Type*)` but don't hit this when I build from source.

## Building + Running

```sh
$ make  # Creates ./main
$ ./main test_input.txt
; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@0 = private unnamed_addr constant [16 x i8] c"Hi! I am ALIVE!\00", align 1

define void @main() {
entry:
  %0 = call i32 @printf(ptr @0)
  ret void
}

declare i32 @printf(ptr %0)
```

Define `EXTRA_CPPFLAGS` to pass flags to the build. For example, to build with
sanitizers:

```
$ make EXTRA_CPPFLAGS="-fsanitize=address -fsanitize=undefined"
```

## Using the compiler

```sh
$ ./main test_input.txt |& llc -filetype=obj -o /tmp/out.o --relocation-model=pic  # Creates a.out
$ ./a.out
Hi! I am ALIVE!
```

## Testing

```sh
$ make test GTEST_HDR=<path to include dir> GTEST_LIB=<path to lib dir>
```

### Reproducing the github actions builders

The github actions can also be tested locally via  docker containers using
[act](https://github.com/nektos/act).

**NOTE**: Docker containers don't have `sudo` by default, so you'll want to
remove all the `sudo`s in the build file you're running.

```
$ act --workflows .github/workflows/build.yml
```
