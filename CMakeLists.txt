cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("FreeRTOS-PlusPlus")

# Initialize variables
set(FRPP_SOURCES "")
set(FRPP_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Process source directories to collect all source files
add_subdirectory(src)

# Export variables to parent scope if this is a subdirectory
get_directory_property(has_parent PARENT_DIRECTORY)
if(has_parent)
  # Export the collected sources and include path to parent project
  set(FRPP_SOURCES ${FRPP_SOURCES} PARENT_SCOPE)
  set(FRPP_INCLUDE_PATH ${FRPP_INCLUDE_PATH} PARENT_SCOPE)
  message(STATUS "FreeRTOS-PlusPlus: Exporting ${FRPP_INCLUDE_PATH} as include path")
  message(STATUS "FreeRTOS-PlusPlus: Found sources: ${FRPP_SOURCES}")
else()
  # Standalone build - add tests
  message(STATUS "FreeRTOS-PlusPlus: Building standalone with tests")
  message(STATUS "FreeRTOS-PlusPlus: Include path: ${FRPP_INCLUDE_PATH}")
  message(STATUS "FreeRTOS-PlusPlus: Sources: ${FRPP_SOURCES}")
  enable_testing()
  add_subdirectory(tests)
endif()


