if (CONFIG_CHRE_WASM)
  set(CXXSRCS)
  list(
    APPEND
    CXXSRCS
    ble_wrapper.cc)
endif()
