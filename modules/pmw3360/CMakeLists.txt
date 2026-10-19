
add_subdirectory(drivers)

if(CONFIG_MONA2_LEGACY_BEHAVIOR_IDS)
  zephyr_library_named(mona2_legacy_behavior_ids)
  zephyr_library_sources(../../compat/legacy_behavior_ids.c)
  zephyr_link_libraries(-Wl,--wrap=zmk_behavior_find_behavior_name_from_local_id)
endif()
