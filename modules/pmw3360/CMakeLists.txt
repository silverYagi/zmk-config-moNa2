
add_subdirectory(drivers)

if(CONFIG_MONA2_LEGACY_BEHAVIOR_IDS)
  # ZMKアプリのヘッダー参照と同じ条件で互換コードをコンパイルする。
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../compat/legacy_behavior_ids.c)
  zephyr_link_libraries(-Wl,--wrap=zmk_behavior_find_behavior_name_from_local_id)
endif()
