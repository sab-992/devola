add_compile_definitions($<$<CONFIG:Debug>:DEBUG_MODE_ENABLED>)

if (WIN32)
    add_compile_definitions(LOG_DIR_PATH="$ENV{LOCALAPPDATA}/${PROJECT_NAME}/Logs")
else()
    add_compile_definitions(LOG_DIR_PATH="$ENV{HOME}/.local/share/${PROJECT_NAME}/logs")
endif()