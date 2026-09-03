function(link_all_libraries target_name libs_dir)
    file(GLOB subdirectories RELATIVE ${libs_dir} "${libs_dir}/*")

    foreach(subdir ${subdirectories})
        set(full_path "${libs_dir}/${subdir}")

        if(IS_DIRECTORY ${full_path})
            get_filename_component(lib_name ${full_path} NAME)
            target_link_libraries(${target_name} PRIVATE "${lib_name}_lib")
        endif()
    endforeach()
endfunction()