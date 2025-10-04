# Collect sources from subdirectories
add_subdirectory(sys)
add_subdirectory(logging)
add_subdirectory(shell)

# Pass the collected sources up to parent scope
set(FRPP_SOURCES ${FRPP_SOURCES} PARENT_SCOPE)
