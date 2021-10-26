# Unit Test Code Coverage

Code coverage for the coreboot unit tests allows us to see what lines of
code in the coreboot library are covered by unit tests, and allows a test
author to see where they need to add test cases for additional coverage.

Code coverage requires `lcov`; install the tool if necessary by
`sudo apt install lcov` or the equivalent for your system.

Enable code coverage in your unit test build by setting the environment
variable `COV` to 1; either `export COV=1` in your shell, or add it to your
`make` command, e.g. `COV=1 make unit-tests`.

The build output directory is either `build/tests` or `build/coverage`,
depending on whether `COV=1` is set in the environment.

All of the unit test targets are available with and without `COV=1`
* `clean-unit-tests`
* `build-unit-tests`
* `run-unit-tests`
* `unit-tests` (which is just `build-unit-tests` followed by `run-unit-tests`)

There are two new `make` targets:
* `coverage-report` generates a code coverage report from all of the
GCOV data (`*.gcda` and `*.gcno` files) in the build directory. To view the
coverage report, open `build/coverage/coverage_reports/index.html` in your web
browser.
* `clean-coverage-report` deletes just the coverage report.

The `coverage-report` and `clean-coverage-report` targets automatically set
`COV=1` if it is not already set in the environment.


## Examples

`COV=1 make unit-tests coverage-report` builds all of the unit tests with code
coverage, runs the unit tests, and generates the code coverage report.

`COV=1 make build-unit-tests` builds all of the unit tests with code coverage.

`COV=1 make run-unit-tests` runs the unit tests, building them with code
coverage if they are out-of-date.

`COV=1 make coverage-report` creates the code coverage report. This
target does not explicitly depend on the tests being built and run; it gathers
the code coverage data from the output directory, which it assumes already
exists.

`COV=1 make tests/lib/uuid-test coverage-report` builds the uuid test
with code coverage, runs it, and generates a code coverage report just for
that test.

As a demonstration that building with and without coverage uses different
output directories:
1. `make build-unit-tests` builds unit tests without code coverage into
`build/tests`.
2. `COV=1 make clean-unit-tests` cleans `build/coverage`
3. `make build-unit-tests` doesn't need to build anything in `build/tests`,
because those files weren't affected by the previous `clean-unit-tests`.
