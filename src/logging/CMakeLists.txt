# Add logging module sources to the list
# Currently no source files in this module
# Uncomment and add files as they are created:
# set(FRPP_SOURCES
#   ${FRPP_SOURCES}
#   ${CMAKE_CURRENT_SOURCE_DIR}/your_logging_file.c
#   PARENT_SCOPE
# )

# Pass through the current source list unchanged
set(FRPP_SOURCES ${FRPP_SOURCES} PARENT_SCOPE)