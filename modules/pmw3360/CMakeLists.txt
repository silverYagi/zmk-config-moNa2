
add_subdirectory(drivers)

if(CONFIG_MONA2_BLE_DIAGNOSTICS)
  zephyr_library_named(mona2_ble_diagnostics)
  zephyr_library_sources(../../diagnostics/ble.c)
endif()
