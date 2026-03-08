function(find_header_files source_dir output_var)
    file(GLOB_RECURSE h_files "${source_dir}/*.h")
    set(${output_var} ${h_files} PARENT_SCOPE)
endfunction()