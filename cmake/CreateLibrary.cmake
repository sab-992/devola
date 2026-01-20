include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FindCppFiles.cmake")

function(create_library lib_path lib_type link_core)
    get_filename_component(lib_name ${lib_path} NAME)
    string(TOLOWER ${lib_name} lib_name_lower)
    set(target_name "${lib_name_lower}_lib")

    set(src_dir "${lib_path}/src")
    set(include_dir "${lib_path}/include")
    
    if(NOT EXISTS ${src_dir})
        message(FATAL_ERROR "Source directory does not exist: ${src_dir}")
    endif()
    
    if(NOT EXISTS ${include_dir})
        message(FATAL_ERROR "Include directory does not exist: ${include_dir}")
    endif()
    
    find_cpp_files(${src_dir} cpp_sources)
    
    if(NOT cpp_sources)
        message(FATAL_ERROR "No .cpp files found in ${src_dir}")
    endif()
    
    message(STATUS "Creating library ${target_name} (${lib_type})")
    
    add_library(${target_name} ${lib_type} ${cpp_sources})

    target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${include_dir}> $<INSTALL_INTERFACE:include>)
    
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON POSITION_INDEPENDENT_CODE ON)

    if(${link_core} AND TARGET core_lib)
        add_dependencies(${target_name} core_lib)
        target_link_libraries(${target_name} PUBLIC core_lib)
    endif()

    if (WIN32)
        set_target_properties(${target_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
endfunction()