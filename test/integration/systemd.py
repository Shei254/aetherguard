#!/usr/bin/env python3

"""Test systemd integration."""

import os
import socket
import tempfile
import time

from testlib import check, path
from testlib.log import log
from testlib.feature import Feature
from testlib.const import MAXSOCKETS
from testlib.proc import aetherguard, Script
from testlib.test import Test


def aetherguardd_start_socket(foo: aetherguard, pass_pid: bool) -> int:
    """Start aetherguardd as systemd socket activation does it."""

    pid = os.fork()
    if not pid:
        env = {**os.environ, "LISTEN_FDS": str(MAXSOCKETS + 1)}
        if pass_pid:
            env["LISTEN_PID"] = str(os.getpid())
        args = [
            path.aetherguardD_PATH,
            "-c",
            foo.work_dir,
            "--pidfile",
            foo.pid_file,
            "--logfile",
            foo.sub("log"),
        ]
        assert not os.execve(path.aetherguardD_PATH, args, env)

    assert pid > 0

    _, status = os.waitpid(pid, 0)
    assert os.WIFEXITED(status)
    return os.WEXITSTATUS(status)


def test_listen_fds(foo: aetherguard) -> None:
    """Test systemd socket activation."""

    foo_log = foo.sub("log")

    log.info("foreground aetherguardd fails with too high LISTEN_FDS")
    status = aetherguardd_start_socket(foo, pass_pid=True)
    check.failure(status)
    check.in_file(foo_log, "Too many listening sockets")

    foo.add_script(Script.aetherguard_UP)
    foo.add_script(Script.aetherguard_DOWN)
    os.remove(foo_log)

    log.info("LISTEN_FDS is ignored without LISTEN_PID")
    status = aetherguardd_start_socket(foo, pass_pid=False)
    foo[Script.aetherguard_UP].wait()
    foo.cmd("stop")
    foo[Script.aetherguard_DOWN].wait()
    check.success(status)
    check.not_in_file(foo_log, "Too many listening sockets")


def recv_until(sock: socket.socket, want: bytes) -> None:
    """Receive from a datagram socket until a specific value is found."""

    while True:
        msg = sock.recv(65000)
        log.info("received %s", msg)
        if msg == want:
            break


def test_watchdog(foo: aetherguard) -> None:
    """Test systemd watchdog."""

    address = tempfile.mktemp()
    foo_log = foo.sub("log")

    log.info("watchdog is disabled if no env vars are passed")
    foo.cmd("start", "--logfile", foo_log)
    foo.cmd("stop")
    check.in_file(foo_log, "Watchdog is disabled")

    log.info("watchdog is enabled by systemd env vars")
    foo.add_script(Script.aetherguard_UP)
    foo.add_script(Script.aetherguard_DOWN)

    with socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM) as sock:
        sock.bind(address)

        watchdog = 0.5
        env = {"NOTIFY_SOCKET": address, "WATCHDOG_USEC": str(int(watchdog * 1e6))}
        proc = foo.aetherguardd("-D", env=env)
        recv_until(sock, b"READY=1")

        for _ in range(6):
            before = time.monotonic()
            recv_until(sock, b"WATCHDOG=1")
            spent = time.monotonic() - before
            assert spent < watchdog

        foo.cmd("stop")
        recv_until(sock, b"STOPPING=1")

        check.success(proc.wait())


with Test("socket activation") as context:
    test_listen_fds(context.node(init=True))

with Test("watchdog") as context:
    node = context.node(init=True)
    if Feature.WATCHDOG in node.features:
        test_watchdog(node)
