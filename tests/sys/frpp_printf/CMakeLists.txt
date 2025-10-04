# Create test executable
add_executable(frpp_printf_tests
  ${FRPP_SOURCES}
  test_frpp_printf.c
)

# Add include directories
target_include_directories(frpp_printf_tests PRIVATE
  ${FRPP_INCLUDE_PATH}
)

# Link Unity framework
target_link_libraries(frpp_printf_tests  PRIVATE
  unity::framework
)

# Set C standard if needed
set_target_properties(frpp_printf_tests PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
)

# Add test
add_test(NAME FreeRTOS_PlusPlus_frpp_printf_tests COMMAND frpp_printf_tests)
