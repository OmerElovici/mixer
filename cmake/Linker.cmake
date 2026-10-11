macro(mixer_configure_linker project_name)
  set(mixer_USER_LINKER_OPTION
    "DEFAULT"
      CACHE STRING "Linker to be used")
    set(mixer_USER_LINKER_OPTION_VALUES "DEFAULT" "SYSTEM" "LLD" "GOLD" "BFD" "MOLD" "SOLD" "APPLE_CLASSIC" "MSVC")
  set_property(CACHE mixer_USER_LINKER_OPTION PROPERTY STRINGS ${mixer_USER_LINKER_OPTION_VALUES})
  list(
    FIND
    mixer_USER_LINKER_OPTION_VALUES
    ${mixer_USER_LINKER_OPTION}
    mixer_USER_LINKER_OPTION_INDEX)

  if(${mixer_USER_LINKER_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
        "Using custom linker: '${mixer_USER_LINKER_OPTION}', explicitly supported entries are ${mixer_USER_LINKER_OPTION_VALUES}")
  endif()

  set_target_properties(${project_name} PROPERTIES LINKER_TYPE "${mixer_USER_LINKER_OPTION}")
endmacro()
