#!/usr/bin/python3
# SPDX-License-Identifier: BSD-3-Clause

import os
import pytest
import struct
import subprocess
from datetime import datetime
from datetime import timedelta

# Defined in include/commonlib/bsd/elog.h
ELOG_TYPE_SYSTEM_BOOT = 0x17
ELOG_TYPE_EOL = 0xff
ELOG_EVENT_HEADER_SIZE = 8
ELOG_EVENT_CHECKSUM_SIZE = 1


def convert_to_event(s: str) -> dict:
    fields = s.split("|")
    assert len(fields) == 3 or len(fields) == 4

    return {
        "index": int(fields[0]),
        "timestamp": datetime.strptime(fields[1].strip(), "%Y-%m-%d %H:%M:%S"),
        "desc": fields[2].strip(),
        "data": fields[3].strip() if len(fields) == 4 else None,
    }


def compare_event(expected: dict, got: dict) -> None:
    # Ignore the keys that might be in "got", but not in "expected".
    # In particular "timestamp" might not want to be tested.
    for key in expected:
        assert key in got.keys()
        assert expected[key] == got[key]


@pytest.fixture(scope="session")
def elogtool_path(request):
    exe = request.config.option.elogtool_path
    assert os.path.exists(exe)
    return exe


@pytest.fixture(scope="function")
def elogfile(tmp_path):
    header_size = 8
    tail_size = 512 - header_size

    # Elog header:
    #  Magic (4 bytes) = "ELOG"
    #  Version (1 byte) = 1
    #  Size (1 byte) = 8
    #  Reserved (2 bytes) = 0xffff
    header = struct.pack("4sBBH", bytes("ELOG", "utf-8"), 1, 8, 0xffff)

    # Fill the tail with EOL events.
    tail = bytes([ELOG_TYPE_EOL] * tail_size)
    buf = header + tail

    buf_path = tmp_path / "elog_empty.bin"
    with buf_path.open("wb") as fd:
        fd.write(buf)
        fd.flush()
        return str(buf_path)
    assert False


def elog_list(elogtool_path: str, path: str) -> list:
    output = subprocess.run([elogtool_path, 'list', '-f', path],
                            capture_output=True, check=True)
    log = output.stdout.decode("utf-8").strip()

    lines = log.splitlines()
    lines = [convert_to_event(s.strip()) for s in lines]
    return lines


def elog_clear(elogtool_path: str, path: str) -> None:
    subprocess.run([elogtool_path, 'clear', '-f', path], check=True)


def elog_add(elogtool_path: str, path: str, typ: int, data: bytearray) -> None:
    subprocess.run([elogtool_path, 'add', '-f', path,
                   hex(typ), data.hex()], check=True)


def test_list_empty(elogtool_path, elogfile):
    events = elog_list(elogtool_path, elogfile)
    assert len(events) == 0


def test_clear_empty(elogtool_path, elogfile):
    elog_clear(elogtool_path, elogfile)
    events = elog_list(elogtool_path, elogfile)

    # Must have one event, the "Log area cleared" event.
    assert len(events) == 1

    expected = {"index": 0,
                "desc": "Log area cleared",
                # "0", since it was an empty elog buffer. No bytes were cleared.
                "data": "0"}
    compare_event(expected, events[0])


def test_clear_not_empty(elogtool_path, elogfile):
    tot_events = 10
    data_size = 4
    event_size = ELOG_EVENT_HEADER_SIZE + data_size + ELOG_EVENT_CHECKSUM_SIZE
    written_bytes = tot_events * event_size

    for i in range(tot_events):
        # Adding boot_count for completeness. But it is ignored in this test.
        boot_count = i.to_bytes(data_size, "little")
        elog_add(elogtool_path, elogfile, ELOG_TYPE_SYSTEM_BOOT, boot_count)
    elog_clear(elogtool_path, elogfile)
    events = elog_list(elogtool_path, elogfile)

    # Must have one event, the "Log area cleared" event.
    assert len(events) == 1

    expected = {"index": 0,
                "desc": "Log area cleared",
                "data": str(written_bytes)
                }
    compare_event(expected, events[0])


def test_add_single_event(elogtool_path, elogfile):
    # "before - one second" is needed because datetime.now() fills the
    # microsecond variable. But eventlog doesn't use it, and has it hardcoded to
    # zero.
    before = datetime.now() - timedelta(seconds=1)
    boot_count = 128
    elog_add(elogtool_path, elogfile, ELOG_TYPE_SYSTEM_BOOT,
             boot_count.to_bytes(4, "little"))
    after = datetime.now()

    events = elog_list(elogtool_path, elogfile)
    assert len(events) == 1

    ev = events[0]
    expected = {"index": 0,
                "desc": "System boot",
                "data": str(boot_count)
                }
    compare_event(expected, ev)

    assert before < ev["timestamp"] < after
