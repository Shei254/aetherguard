#!/usr/bin/env python3

"""Check that basic functionality works (aetherguardd can be started and stopped)."""

from testlib.test import Test
from testlib.proc import aetherguard
from testlib.feature import SANDBOX_LEVEL
from testlib.log import log
from testlib.script import Script
from testlib import check


def init(ctx: Test) -> aetherguard:
    """Initialize new test nodes."""
    node = ctx.node(init=f"set Sandbox {SANDBOX_LEVEL}")
    node.add_script(Script.aetherguard_UP)
    return node


def test(ctx: Test, *flags: str) -> None:
    """Run tests with flags."""
    log.info("init new node")
    node = init(ctx)

    log.info('starting aetherguardd with flags "%s"', " ".join(flags))
    aetherguardd = node.aetherguardd(*flags)

    log.info("waiting for aetherguard-up script")
    node[Script.aetherguard_UP].wait()

    log.info("stopping aetherguardd")
    node.cmd("stop")

    log.info("checking aetherguardd exit code")
    check.success(aetherguardd.wait())


with Test("foreground mode") as context:
    test(context, "-D")

with Test("background mode") as context:
    test(context)
