"""Some hardcoded constants."""

from .proc import Feature, aetherguard

# True if aetherguardd has sandbox support
HAVE_SANDBOX = Feature.SANDBOX in aetherguard().features

# Maximum supported sandbox level
SANDBOX_LEVEL = "high" if Feature.SANDBOX in aetherguard().features else "off"
