# SPDX-License-Identifier: BSD-3-Clause

import pathlib


def pytest_addoption(parser):
    here = pathlib.Path(__file__).parent
    parser.addoption(
        "--elogtool-path",
        type=pathlib.Path,
        default=(here / ".." / "elogtool").resolve(),
    )
