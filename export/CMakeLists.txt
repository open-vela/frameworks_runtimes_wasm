if (CONFIG_LIGHTWEIGHT_WASM_CONTAINER)
  set(CXXSRCS)
  list(APPEND CFLAGS ${INCDIR_PREFIX}$(APPDIR)/frameworks/wasm/export/include)
  list(APPEND CXXFLAGS ${INCDIR_PREFIX}$(APPDIR)/frameworks/wasm/export/include)
  list(
    APPEND
    CXXSRCS
    wasm_call_native_api.cc)
endif()
