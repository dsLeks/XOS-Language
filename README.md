# XOS-Language

XOS? SAUCE?

![Build Status](https://github.com/github/docs/actions/workflows/build.yml/badge.svg)

## Min Requirements

This could potentially build with earlier versions. These just happen to be the
ones tested with locally.

- gcc v13
- make v4
- [googletest v1.12.1](https://github.com/google/googletest/releases/tag/release-1.12.1)  (only needed for testing)

## Building + Running

```sh
$ make
./main
```

## Testing

```sh
$ make test GTEST_HDR=<path to include dir> GTEST_LIB=<path to lib dir>
```
