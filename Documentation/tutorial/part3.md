# Writing unit tests for coreboot

## Introduction
General thoughts about unit testing coreboot can be found in [Unit
testing coreboot](../technotes/2020-03-unit-testing-coreboot.md).
Additionally, [code coverage](../technotes/2021-05-code-coverage.md)
support is available for unit tests.

This document aims to guide developers through the process of adding and
writing unit tests for coreboot modules.

As an example of unit under test, `src/device/i2c.c` (referred hereafter
as UUT "Unit Under Test") will be used. This is simple module, thus it
should be easy for the reader to focus solely on the testing logic,
without the need to spend too much time on digging deeply into the
source code details and flow of operations. That being said, a good
understanding of what the unit under test is doing is crucial for
writing unit tests.

This tutorial should also be helpful for developers who want to follow
[TDD](https://en.wikipedia.org/wiki/Test-driven_development). Even
though TDD has a different work flow of building tests first, followed
by the code that satisfies them, the process of writing tests and adding
them to the tree is the same.

## Analysis of unit under test
First of all, it is necessary to precisely establish what we want to
test in a particular module. Usually this will be an externally exposed
API, which can be used by other modules.

```eval_rst
.. admonition:: i2c-test example

   In case of our UUT, API consist of two methods:

   .. code-block:: c

     int i2c_read_field(unsigned int bus, uint8_t chip, uint8_t reg,
                 uint8_t *data, uint8_t mask, uint8_t shift)
     int i2c_write_field(unsigned int bus, uint8_t chip, uint8_t reg,
                 uint8_t data, uint8_t mask, uint8_t shift)

   For sake of simplicity, let's focus on `i2c_read_field` in this
   document.
```

Once the API is defined, the next question is __what__ this API is doing
(or what it will be doing in case of TDD). In other words, what outputs
we are expecting from particular functions, when providing particular
input parameters.

```eval_rst
.. admonition:: i2c-test example

   .. code-block:: c

     int i2c_read_field(unsigned int bus, uint8_t chip, uint8_t reg,
                 uint8_t *data, uint8_t mask, uint8_t shift)

   This is a method which means to read content of register `reg` from
   i2c device on i2c `bus` and slave address `chip`, applying bit `mask`
   and offset `shift` to it. Returned data should be placed in `data`.
```

The next step is to determine all external dependencies of UUT in order
to mock them out. Usually we want to isolate the UUT as much as
possible, so that the test result depends __only__ on the behavior of
UUT and not on the other modules. While some software dependencies may
be hard to be mock (for example due to complicated dependencies) and
thus should be simply linked into the test binaries, all hardware
dependencies need to be mocked out, since in the user-space host
environment, targets hardware is not available.

```eval_rst
.. admonition:: i2c-test example

   `i2c_read_field` is calling `i2c_readb`, which eventually invokes
   `i2c_transfer`. This method simply calls `platform_i2c_transfer`. The
   last function in the chain is a hardware-touching one, and defined
   separately for different SOCs. It is responsible for issuing
   transactions on the i2c bus.  For the purpose of writing unit test,
   we should mock this function.
```

## Adding new tests
In order to keep the tree clean, the `tests/` directory should mimic the
`src/` directory, so that test harness code is placed in a location
corresponding to UUT. Furthermore, the naming convention is to add the
suffix `-test` to the UUT name when creating a new test harness file.

```eval_rst
.. admonition:: i2c-test example

   Considering that UUT is `src/device/i2c.c`, test file should be named
   `tests/device/i2c-test.c`. When adding a new test file, it needs to
   be registered with the coreboot unit testing infrastructure.
```

Every directory under `tests/` should contain a Makefile.inc, similar to
what can be seen under the `src/`. Register a new test in Makefile.inc,
by __appending__ test name to the `tests-y` variable.

```eval_rst
.. admonition:: i2c-test example

   .. code-block:: c

     tests-y += i2c-test
```

Next step is to list all source files, which should be linked together
in order to create test binary. Usually a tests requires only two files
- UUT and test harness code, but sometimes more is needed to provide the
test environment.  Source files are registered in `<test_name>-srcs`
variable.

```eval_rst
.. admonition:: i2c-test example

   .. code-block:: c

     i2c-test-srcs += tests/device/i2c-test.c
     i2c-test-srcs += src/device/i2c.c
```

Above minimal configuration is a basis for further work. One can try to
build and run test binary either by invoking `make
tests/<test_dir>/<test_name>` or by running all unit tests (whole suite)
for coreboot `make unit-tests`.

```eval_rst
.. admonition:: i2c-test example

   .. code-block:: c

     make tests/device/i2c-test

   or

   .. code-block:: c

     make unit-tests
```

When trying to build test binary, one can often see linker complains
about `undefined reference` to couple of symbols. This is one of
solutions to determine all external dependencies of UUT - iteratively
build test and resolve errors one by one. At this step, developer should
decide either it's better to add an extra module to provide necessary
definitions or rather mock such dependency. Quick guide through adding
mocks is provided later in this doc.

## Writing new tests
In coreboot, [Cmocka](https://cmocka.org/) is used as unit test
framework. The project has exhaustive [API
documentation](https://api.cmocka.org/). Let's see how we may
incorporate it when writing tests.

### Assertions
Testing the UUT consists of calling the functions in the UUT and
comparing the returned values to the expected values. Cmocka implements
[a set of assert
macros](https://api.cmocka.org/group__cmocka__asserts.html) to compare a
value with an expected value. If the two values do not match, the test
fails with an error message.

```eval_rst
.. admonition:: i2c-test example

   In our example, the simplest test is to call UUT for reading our fake
   devices registers and do all calculation in the test harness itself.
   At the end, let's compare integers with `assert_int_equal`.

   .. code-block:: c

     #define MASK        0x3
     #define SHIFT        0x1

     static void i2c_read_field_test(void **state)
     {
             int bus, slave, reg;
             int i, j;
             uint8_t buf;

             mock_expect_params_platform_i2c_transfer();

             /* Read particular bits in all registers in all devices, then compare
                with expected value. */
             for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++)
                     for (j = 0; j < ARRAY_SIZE(i2c_ex_devs[0].regs); j++) {
                             i2c_read_field(i2c_ex_devs[i].bus,
                                     i2c_ex_devs[i].slave,
                                     i2c_ex_devs[i].regs[j].reg,
                                     &buf, MASK, SHIFT);
                             assert_int_equal((i2c_ex_devs[i].regs[j].data &
                                     (MASK << SHIFT)) >> SHIFT, buf);
                     };
     }
```

### Mocks

#### Overview
Many coreboot modules are low level software that touch hardware
directly.  Because of this, one of the most important and challenging
part of writing tests is to design and implement mocks. A mock is a
software component which implements the API of another component so that
the test can verify that certain functions are called (or not called),
verify the parameters passed to those functions, and specify the return
values from those functions. Mocks are especially useful when the API to
be implemented is one that accesses hardware components.

When writing a mock, the developer implements the same API as the module
being mocked. Such a mock may, for example, register a set of driver
methods. Behind this API, there is usually a simulation of real
hardware.

```eval_rst
.. admonition:: i2c-test example

   For purpose of our i2c test, we may introduce two i2c devices with
   set of registers, which simply are structs in memory.

   .. code-block:: c

     /* Simulate two i2c devices, both on bus 0, each with three uint8_t regs
        implemented. */
     typedef struct {
             uint8_t reg;
             uint8_t data;
     } i2c_ex_regs_t;

     typedef struct {
             unsigned int bus;
             uint8_t slave;
             i2c_ex_regs_t regs[3];
     } i2c_ex_devs_t;

     i2c_ex_devs_t i2c_ex_devs[] = {
             {.bus = 0, .slave = 0xA, .regs = {
                     {.reg = 0x0, .data = 0xB},
                     {.reg = 0x1, .data = 0x6},
                     {.reg = 0x2, .data = 0xF},
             } },
             {.bus = 0, .slave = 0x3, .regs = {
                     {.reg = 0x0, .data = 0xDE},
                     {.reg = 0x1, .data = 0xAD},
                     {.reg = 0x2, .data = 0xBE},
             } },
     };

   These fake devices will be accessed instead of hardware ones:

   .. code-block:: c

             reg = tmp->buf[0];

             /* Find object for requested device */
             for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++, i2c_dev++)
                     if (i2c_ex_devs[i].slave == tmp->slave) {
                             i2c_dev = &i2c_ex_devs[i];
                             break;
                     }

             if (i2c_dev == NULL)
                     return -1;

             /* Write commands */
             if (tmp->len > 1) {
                     i2c_dev->regs[reg].data = tmp->buf[1];
             };

             /* Read commands */
             for (i = 0; i < count; i++, tmp++)
                     if (tmp->flags & I2C_M_RD) {
                             *(tmp->buf) = i2c_dev->regs[reg].data;
                     };
```

Cmocka uses a feature that gcc provides for breaking dependencies at the
link time. It is possible to override implementation of some function,
with the method from test harness. This allows test harness to take
control of execution from binary (during the execution of test), and
stimulate UUT as required without changing the source code.

coreboot unit test infrastructure supports overriding of functions at
link time.  This is as simple as adding a `name_of_function` to be
mocked into <test_name>-mocks variable in Makefile.inc. The result is
that the test's implementation of that function is called instead of
coreboot's.

```eval_rst
.. admonition:: i2c-test example

   .. code-block:: c

     i2c-test-mocks += platform_i2c_transfer

   Now, dev can write own implementation of `platform_i2c_transfer`.
   This implementation instead of accessing real i2c bus, will
   write/read from fake structs.

   .. code-block:: c

     int platform_i2c_transfer(unsigned int bus, struct i2c_msg
                 *segments, int count)
     {
     }
```

#### Checking mock's arguments
A test can verify the parameters provided by the UUT to the mock
function. The developer may also verify that number of calls to mock is
correct and the order of calls to particular mocks is as expected (See
[this](https://api.cmocka.org/group__cmocka__call__order.html)). The
Cmocka macros for checking parameters are described
[here](https://api.cmocka.org/group__cmocka__param.html). In general, in
mock function, one makes a call to `check_expected(<param_name>)` and in
the corresponding test function, `expect*()` macro, with description
which parameter in which mock should have particular value, or be inside
a described range.

```eval_rst
.. admonition:: i2c-test example

   In our example, we may want to check that `platform_i2c_transfer` is
   fed with number of segments bigger than 0, each segment has flags
   which are in supported range and each segment has buf which is
   non-NULL. We are expecting such values for _every_ call, thus the
   last parameter in `expect*` macros is -1.

   .. code-block:: c

     static void mock_expect_params_platform_i2c_transfer(void)
     {
             unsigned long int expected_flags[] = {0, I2C_M_RD,
                     I2C_M_TEN, I2C_M_RECV_LEN, I2C_M_NOSTART};

             /* Flags should always be only within supported range */
             expect_in_set_count(platform_i2c_transfer, segments->flags,
                     expected_flags, -1);

             expect_not_value_count(platform_i2c_transfer, segments->buf,
                     NULL, -1);

             expect_in_range_count(platform_i2c_transfer, count, 1,
                     INT_MAX, -1);
     }

   And the checks below should be added to our mock

   .. code-block:: c

             check_expected(count);

             for (i = 0; i < count; i++, segments++) {
                     check_expected_ptr(segments->buf);
                     check_expected(segments->flags);
             }
```

#### Instrument mocks
It is possible for the test function to instrument what the mock will
return to the UUT. This can be done by using the `will_return*()` and
`mock()` macros.  These are described in [the Mock Object
section](https://api.cmocka.org/group__cmocka__mock.html) of the Cmocka
API documentation.

```eval_rst
.. admonition:: Example

   There is an non-coreboot example for using Cmocka available
   `here <https://lwn.net/Articles/558106/>`_.
```

### Test runner
Finally, the developer needs to implement the test `main()` function.
All tests should be registered there and cmocka test runner invoked. All
methods for invoking Cmocka test are described
[here](https://api.cmocka.org/group__cmocka__exec.html).

```eval_rst
.. admonition:: i2c-test example

   We don't need any extra setup and teardown functions for i2c-test, so
   let's simply register test for `i2c_read_field` and return from main
   value which is output of Cmocka's runner (it returns number of tests
   that failed).

   .. code-block:: c

     int main(void)
     {
             const struct CMUnitTest tests[] = {
                     cmocka_unit_test(i2c_read_field_test),
             };

             return cb_run_group_tests(tests, NULL, NULL);
     }
```
