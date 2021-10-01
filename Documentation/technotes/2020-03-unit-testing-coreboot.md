# Unit testing coreboot

## Preface
First part of this document, Introduction, comprises disambiguation for what
unit testing is and what is not. This definition will be a basis for the whole
paper.

Next, Rationale, explains why to use unit testing and how coreboot specifically
may benefit from it.

This is followed by evaluation of different available free C unit test
frameworks. Firstly, collection of requirements is provided. Secondly, there is
a description of a few selected candidates. Finally, requirements are applied to
candidates to see if they might be a good fit.

Fourth part is a summary of evaluation, with proposal of unit test framework
for coreboot to be used.

Finally, Implementation proposal paragraph touches how build system and coreboot
codebase in general should be organized, in order to support unit testing. This
comprises couple of design considerations which need to be addressed.

## Introduction
A unit test is supposed to test a single unit of code in isolation. In C
language (in contrary to OOP) unit usually means a function. One may also
consider unit under test to be a single compilation unit which exposes some
API (set of functions). A function, talking to some external component can be
tested if this component can be mocked out.

In other words (looking from C compilation angle), there should be no extra
dependencies (executables) required beside unit under test and test harness in
order to compile unit test binary. Test harness, beside code examining a
routines, may comprise test framework implementation.

It is hard to apply this strict definition of unit test to firmware code in
practice, mostly due to constraints on speed of execution and size of final
executable. coreboot codebase often cannot be adjusted to be testable. Because
of this, coreboot unit testing subsystem should allow to include some additional
source object files beside unit under test. That being said, the default and
goal wherever possible, should be to isolate unit under test from other parts.

Unit testing is not an integration testing and it doesn't replace it. First of
all, integration tests cover larger set of components and interactions between
them. Positive integration test result gives more confidence than a positive
unit test does. Furthermore, unit tests are running on the build machine, while
integration tests usually are executed on the target (or simulator).

## Rationale
Considering above, what is the benefit of unit testing, especially keeping in
mind that coreboot is low-level firmware? Unit tests should be quick, thus may
be executed frequently during development process. It is much easier to build
and run a unit test on a build machine, than any integration test. This in turn
may be used by dev to gather extra confidence early during code development
process. Actually developer may even write unit tests earlier than the code -
see [TDD](https://en.wikipedia.org/wiki/Test-driven_development) concept.

That being said, unit testing embedded C code is a difficult task, due to
significant amount of dependencies on underlying hardware. Mocking can handle
some hardware dependencies. However, complex mocks make the unit test
susceptible to failing and can require significant development effort.

Writing unit tests for a code (both new and currently existing) may be favorable
for the code quality. It is not only about finding bugs, but in general - easily
testable code is a good code.

coreboot benefits the most from testing common libraries (lib/, commonlib/,
payloads/libpayload) and coreboot infrastructure (console/, device/, security/).

## Evaluation of unit testing frameworks

### Requirements
Requirements for unit testing frameworks:

* Easy to use
* Few dependencies

    Standard C library is all we should need

* Isolation between tests
* Support for mocking
* Support for some machine parsable output
* Compiler similarity

   Compiler for the host _must_ support the same language standards as the target
   compiler. Ideally the same toolchain should be used for building firmware
   executables and test binaries, however the host compiler will be used to build
   unit tests, whereas the coreboot toolchain will be used for building the
   firmware executables. For some targets, the host compiler and the target
   compiler could be the same, but this is not a requirement.

* Same language for tests and code

   Unit tests will be written in C, because coreboot code is also written in C

### Desirables

* Easy to integrate with build system/build tools

   Ideally JUnit-like XML output format for Jenkins

* Popularity is a plus

   We want a larger community for a couple of reasons. Firstly, easier access to
   people with knowledge and tutorials. Secondly, bug fixes for the top of tree
   are more frequent and known issues are usually shorter in the pending state.
   Last but not least, larger reviewer pool means better and easier upstream
   improvements that we would like to submit.

* Extra features may be a plus
* Compatible license

   This should not be a blocker, since test binaries are not distributed.
   However ideally compatible with GPL.

* IDE integration

### Candidates
There is a lot of frameworks which allow unit testing C code
([list](https://en.wikipedia.org/wiki/List_of_unit_testing_frameworks#C) from
Wikipedia). While not all of them were evaluated, because that would take an
excessive amount of time, couple of them were selected based on the good
opinions among C devs, popularity and fitting above criteria.

* [SputUnit](https://www.use-strict.de/sput-unit-testing/)
* [GoogleTest](https://github.com/google/googletest)
* [Cmocka](https://cmocka.org/)
* [Unity](http://www.throwtheswitch.org/unity) (CMock, Ceedling)

We looked at several other test frameworks, but decided not to do a full evaluation
for various reasons such as functionality, size of the developer community, or
compatibility.

### Evaluation
* [SputUnit](https://www.use-strict.de/sput-unit-testing/)
  * Pros
    * No dependencies, one header file to include - that’s all
    * Pure C
    * Very easy to use
    * BSD license
  * Cons
    * Main repo doesn’t have support for generating JUnit XML reports for
      Jenkins to consume - this feature is available only on the fork from
      SputUnit called “Sput_report”. It makes it niche in a niche, so there are
      some reservations whether support for this will be satisfactory
    * No support for mocks
    * Not too popular
    * No automatic test registration
* [GoogleTest](https://github.com/google/googletest)
  * Pros
    * Automatic test registration
    * Support for different output formats (including XML for Jenkins)
    * Good support, widely used, the biggest and the most active community out
      of all frameworks that were investigated
    * Available as a package in the most common distributions
    * Test fixtures easily available
    * Well documented
    * Easy to integrate with an IDE
    * BSD license
  * Cons
    * Requires C++11 compiler
    * To make most out of it (use GMock) C++ knowledge is required
* [Cmocka](https://cmocka.org/)
  * Pros
    * Self-contained, autonomous framework
    * Pure C
    * API is well documented
    * Multiple output formats (including XML for Jenkins)
    * Available as a package in the most common distributions
    * Used in some popular open source projects (libssh, OpenVPN, Samba)
    * Test fixtures available
    * Support for exception handling
  * Cons
    * No automatic test registration
    * It will require some effort to make it work from within an IDE
    * Apache 2.0 license (not compatible with GPLv2)
* [Unity](http://www.throwtheswitch.org/unity) (CMock, Ceedling)
  * Pros
    * Pure C (Unity testing framework itself, not test runner)
    * Support for different output formats (including XML for Jenkins)
    * There are some (rather easy) hints how to use this from an IDE (e.g. Eclipse)
    * MIT license
  * Cons
    * Test runner (Ceedling) is not written in C - uses Ruby
    * Mocking/Exception handling functionalities are actually separate tools
    * No automatic test registration
    * Not too popular

### Summary & framework proposal
After research, we propose using the Cmocka unit test framework. Cmocka fulfills
all stated evaluation criteria. It is rather easy to use, doesn’t have extra
dependencies, written fully in C, allows for tests fixtures and some popular
open source projects already are using it. Cmocka also includes support for
mocks.

Cmocka's limitations, such as the lack of automatic test registration, are
considered minor issues that will require only minimal additional work from a
developer. At the same time, it may be worth to propose improvement to Cmocka
community or simply apply some extra wrapper with demanded functionality.

## Implementation

### Framework as a submodule or external package
Unit test frameworks may be either compiled from source (from a git submodule
under 3rdparty/) or pre-compiled as a package. The second option seems to be
easier to maintain, while at the same time may bring some unwanted consequences
(different version across distributions, frequent changes in API). It makes sense
to initially experiment with packages and check how it works. If this will
cause any issues, then it is always possible to switch to submodule approach.

### Integration with build system
To get the most out of unit testing framework, it should be integrated with
Jenkins automation server. Verification of all unit tests for new changes may
improve code reliability to some extent.

### Build configuration (Kconfig)
While building unit under test object file, it is necessary to apply some
configuration (config) just like when building usual firmware. For simplicity,
there will be one default tests .config `qemu_x86_i440fx` for all unit tests. At
the same time, some tests may require running with different values of particular
config. This should be handled by adding extra header, included after config.h.
This header will comprise #undef of old CONFIG values and #define of the
required value. When unit testing will be integrated with Jenkins, it may be
preferred to use every available config for periodic builds.

### Directory structure
Tests should be kept separate from the code, while at the same time it must be
easy to match code with test harness.

We create new directory for test files ($(toplevel)/tests/) and mimic the
structure of src/ directory.

Test object files (test harness, unit under tests and any additional executables
are stored under build/tests/<test_name> directory.

Below example shows how directory structure is organized for the two test cases:
tests/lib/string-test and tests/device/i2c-test:

```bash
├── src
│   ├── lib
│   │   ├── string.c <- unit under test
│   │
│   ├── device
│       ├── i2c.c
│
├── tests
│   ├── include
│   │   ├── mocks <- mock headers, which replace original headers
│   │
│   ├── Makefile.inc <- top Makefile for unit tests subsystem
│   ├── lib
│   │   ├── Makefile.inc
│   │   ├── string-test.c <- test code for src/lib/string.c
│   │   │
│   ├── device
│   │   ├── Makefile.inc
│       ├── i2c-test.c
│
├── build
│   ├── tests <-all test-related executables
        ├── config.h <- default config used for tests builds
        ├── lib
        │   ├── string-test <- all string-test executables
        │   │   ├── run     <- final test binary
        │   │   ├── tests   <- all test harness executables
        │   │       ├── lib
        │   │           ├── string-test.o  <-test harness executable
        │   │   ├── src    <- unit under test and other src executables
        │   │       ├── lib
        │   │           ├── string.o       <- unit under test executable
        ├── device
            ├── i2c-test
                ├── run
                ├── tests
                │   ├── device
                │       ├── i2c-test.o
                ├── src
                    ├── device
                        ├── i2c.o
```

### Writing new tests
Our tutorial series has [detailed guidelines](../tutorial/part3.md) for writing
unit tests.
