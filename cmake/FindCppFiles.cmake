function(find_cpp_files source_dir output_var)
    file(GLOB_RECURSE cpp_files "${source_dir}/*.cpp")
    set(${output_var} ${cpp_files} PARENT_SCOPE)
endfunction()