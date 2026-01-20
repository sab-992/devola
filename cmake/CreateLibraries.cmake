include("${CMAKE_CURRENT_LIST_DIR}/../cmake/CreateLibrary.cmake")

set(exclude_dirs "core" "database")

function(create_libraries libs_dir)
    file(GLOB subdirectories RELATIVE ${libs_dir} "${libs_dir}/*")

    foreach(subdir ${subdirectories})
        set(full_path "${libs_dir}/${subdir}")

        list(FIND exclude_dirs ${subdir} index)
        if(IS_DIRECTORY "${full_path}" AND index LESS_EQUAL -1)
            create_library(${full_path} STATIC TRUE)
        endif()
    endforeach()
endfunction()