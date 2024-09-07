dep_tunemu = dependency('tunemu', required: opt_tunemu, static: static)
dep_pcap = dependency('pcap', required: opt_tunemu, static: static)

if dep_tunemu.found() and dep_pcap.found()
  deps_aetherguardd += [dep_tunemu, dep_pcap]
  src_aetherguardd += files('tunemu.c')
  cdata.set('ENABLE_TUNEMU', 1)
endif

# macOS apparently doesn't support kqueue with TAP devices
src_aetherguardd += src_event_select

