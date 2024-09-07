"""Test context that wraps aetherguard instances and terminates them on exit."""

import typing as T

from .log import log
from .proc import aetherguard


class Test:
    """Test context. Allows you to obtain aetherguard instances which are automatically
    stopped (and killed if necessary) at __exit__. Should be wrapped in `with`
    statements (like the built-in `open`). Should be used sparingly (as it usually
    happens, thanks to Windows: service registration and removal is quite slow,
    which makes tests take a long time to run, especially on modest CI VMs).
    """

    name: str
    _nodes: T.List[aetherguard]

    def __init__(self, name: str) -> None:
        self._nodes = []
        self.name = name

    def node(self, addr: str = "", init: T.Union[str, bool] = "") -> aetherguard:
        """Create a aetherguard instance and remember it for termination on exit."""
        node = aetherguard(addr=addr)
        self._nodes.append(node)
        if init:
            if isinstance(init, bool):
                init = ""
            stdin = f"""
                init {node}
                set Port 0
                set Address localhost
                set DeviceType dummy
                {init}
            """
            node.cmd(stdin=stdin)
        return node

    def __str__(self) -> str:
        return self.name

    def __enter__(self) -> "Test":
        log.info("RUNNING TEST: %s", self.name)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        for node in self._nodes:
            node.cleanup()
        log.info("FINISHED TEST: %s", self.name)
