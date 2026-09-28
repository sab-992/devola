add_compile_definitions($<$<CONFIG:Debug>:DEBUG_MODE_ENABLED>)

add_compile_definitions(LOG_DIR_PATH="$ENV{HOME}/.local/share/${PROJECT_NAME}/logs")