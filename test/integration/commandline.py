#!/usr/bin/env python3

"""Test supported and unsupported commandline flags."""

import os
import shutil
import signal
import subprocess as subp
import tempfile
import time

from testlib import check, util, path
from testlib.log import log
from testlib.proc import aetherguard, Script
from testlib.test import Test
from testlib.feature import SANDBOX_LEVEL


def init(ctx: Test) -> aetherguard:
    """Initialize new test nodes."""
    aetherguard = ctx.node(init=f"set Sandbox {SANDBOX_LEVEL}")
    aetherguard.add_script(Script.aetherguard_UP)
    return aetherguard


with Test("commandline flags") as context:
    node = init(context)
    pf = node.pid_file

    aetherguardd_flags = (
        (0, ("-D",)),
        (0, ("--no-detach",)),
        (0, ("-D", "-d")),
        (0, ("-D", "-d2")),
        (0, ("-D", "-d", "2")),
        (0, ("-D", "-n", "foo")),
        (0, ("-D", "-nfoo")),
        (0, ("-D", "--net=foo")),
        (0, ("-D", "--net", "foo")),
        (0, ("-D", "-c", ".", "-c", ".")),
        (0, ("-D", "-n", "net", "-n", "net")),
        (0, ("-D", "-n", "net", "-o", "FakeOpt=42")),
        (0, ("-D", "--logfile=log", "--logfile=log")),
        (0, ("-D", f"--pidfile={pf}", f"--pidfile={pf}")),
        (1, ("foo",)),
        (1, ("--pidfile",)),
        (1, ("--foo",)),
        (1, ("-n", "net", "-o", "Compression=")),
        (1, ("-c", "fakedir", "-n", "n/e\\t")),
    )

    for code, flags in aetherguardd_flags:
        COOKIE = util.random_string(10)
        server = node.aetherguardd(*flags, env={"COOKIE": COOKIE})

        if not code:
            log.info("waiting for aetherguardd to come up")
            env = node[Script.aetherguard_UP].wait().env
            check.equals(COOKIE, env["COOKIE"])

        log.info("stopping aetherguard")
        node.cmd("stop", code=code)

        log.info("reading aetherguardd output")
        stdout, stderr = server.communicate()

        log.debug('got code %d, ("%s", "%s")', server.returncode, stdout, stderr)
        check.equals(code, server.returncode)

    aetherguard_flags = (
        (0, ("get", "name")),
        (0, ("-n", "foo", "get", "name")),
        (0, ("-nfoo", "get", "name")),
        (0, ("--net=foo", "get", "name")),
        (0, ("--net", "foo", "get", "name")),
        (0, ("-c", "conf", "-c", "conf")),
        (0, ("-n", "net", "-n", "net")),
        (0, (f"--pidfile={pf}", f"--pidfile={pf}")),
        (1, ("-n", "foo", "get", "somethingreallyunknown")),
        (1, ("--net",)),
        (1, ("--net", "get", "name")),
        (1, ("foo",)),
        (1, ("-c", "conf", "-n", "n/e\\t")),
    )

    for code, flags in aetherguard_flags:
        node.cmd(*flags, code=code)


def test_relative_path(ctx: Test, chroot: bool) -> None:
    """Test aetherguardd with relative paths."""

    foo = init(ctx)
    confdir = os.path.realpath(foo.sub("."))

    # Workaround for the 108-char limit on UNIX socket path length.
    shortcut = tempfile.mkdtemp()
    os.symlink(confdir, os.path.join(shortcut, "conf"))

    log.info("using paths: confdir '%s', shortcut '%s'", confdir, shortcut)

    args = [
        path.aetherguardD_PATH,
        "-D",
        "-c",
        "conf",
        "--pidfile",
        "pid",
        "--logfile",
        "conf/.//./log",
    ]

    if chroot:
        args.append("-R")

    pidfile = os.path.join(shortcut, "pid")
    logfile = os.path.join(confdir, "log")

    with subp.Popen(args, stderr=subp.STDOUT, cwd=shortcut) as aetherguardd:
        foo[Script.aetherguard_UP].wait(10)

        log.info("pidfile and logfile must exist at expected paths")
        check.file_exists(pidfile)
        check.file_exists(logfile)

        # chrooted aetherguardd won't be able to reopen its log since in this
        # test we put the log outside aetherguard's configuration directory.
        if os.name != "nt" and not chroot:
            log.info("test log file rotation")
            time.sleep(1)
            util.remove_file(logfile)
            os.kill(aetherguardd.pid, signal.SIGHUP)
            time.sleep(1)

            log.info("pidfile and logfile must still exist")
            check.file_exists(pidfile)
            check.file_exists(logfile)

        log.info("stopping aetherguard through '%s'", pidfile)
        foo.cmd("--pidfile", pidfile, "stop")
        check.success(aetherguardd.wait())

    # Leave behind as debugging aid if there's an exception
    shutil.rmtree(shortcut)


with Test("relative path to aetherguardd dir") as context:
    test_relative_path(context, chroot=False)

if os.name != "nt" and not os.getuid():
    with Test("relative path to aetherguardd dir (chroot)") as context:
        test_relative_path(context, chroot=True)
