function(find_cpp_files SOURCE_DIR OUTPUT_VAR)
    file(GLOB_RECURSE CPP_FILES "${SOURCE_DIR}/*.cpp")
    set(${OUTPUT_VAR} ${CPP_FILES} PARENT_SCOPE)
endfunction()
