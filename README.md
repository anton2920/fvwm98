# FVWM98

This repository contains fvwm98 as found at https://downloads.sourceforge.net/project/fvwm95/fvwm98/2.0.43b/fvwm98_2.0.43b.orig.tgz with additional quality of life improvements.

Following changes were made:

- Revised compiler flags so modern `clang` can build it.
- Fixed the least annoying compiler warnings, which in turns fixed a few crashes.
- Added personal configuration as _sample.fvwmrc/anton2920.fvwm95rc_.
- Other bugfixes and improvements.

Tested with:

```
$ uname -biKmoprsv
FreeBSD 13.1-RELEASE FreeBSD 13.1-RELEASE releng/13.1-n250148-fc952ac2212 GENERIC amd64 amd64 GENERIC 1301000 5b12d97f9629e35915bb91fb9145e851f0b437fe
```

```
$ clang -v
FreeBSD clang version 13.0.0 (git@github.com:llvm/llvm-project.git llvmorg-13.0.0-0-gd7b669b3a303)
Target: x86_64-unknown-freebsd13.1
Thread model: posix
InstalledDir: /usr/bin
```

Original README is available as _README.hector_.
