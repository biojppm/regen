
function(download_file_with_name url dest_file)
    if(NOT EXISTS ${dest_file})
        get_filename_component(dest_dir ${dest_file} DIRECTORY)
        if(NOT EXISTS ${dest_dir})
            file(MAKE_DIRECTORY ${dest_dir})
        endif()
        message(STATUS "downloading ${url} ... ${dest_file}")
        file(DOWNLOAD ${url} ${dest_file})
        message(STATUS "finished downloading: ${dest_file}")
    endif()
endfunction()

function(download_file url dest_dir)
    get_filename_component(basename ${url} NAME)
    set(dest ${dest_dir}/${basename})
    download_file_with_name(${url} ${dest})
endfunction()

function(unpack_to file dir)
    get_filename_component(basename ${file} NAME)
    set(_mark ${CMAKE_CURRENT_BINARY_DIR}/${basename}.done)
    if(NOT EXISTS "${_mark}")
        message(STATUS "unpacking ${file} ----> ${dir}")
        get_filename_component(workdir ${file} DIRECTORY)
        set(workdir ${workdir}/unpack-temp)
        message(STATUS "workdir: ${workdir}")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory ${workdir})
        execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xfJ ${file}
            WORKING_DIRECTORY ${workdir})
        #
        get_filename_component(basename ${file} NAME)
        string(REGEX REPLACE ".tar.xz$" "" basename "${basename}")
        if(NOT EXISTS ${dir})
            message(STATUS "renaming ${workdir} ... ${dir}")
            get_filename_component(parent ${dir} DIRECTORY)
            execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory ${parent})
            execute_process(COMMAND "${CMAKE_COMMAND}" -E rename ${workdir}/${basename} ${dir})
        else()
            message(STATUS "copying ${workdir}/${basename}/* to ${dir}")
            file(GLOB files
                LIST_DIRECTORIES true
                #RELATIVE "${workdir}/${basename}"
                "${workdir}/${basename}/*")
            message(STATUS "file list: ${files}")
            # for some reason the next command does nothing...
            #execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${files}" "${dir}")
            # ... so brute force it by using mv directly. FIX THIS.
            execute_process(COMMAND mv ${files} ${dir})
            if(workdir STREQUAL "")
                message(FATAL_ERROR "dont remove any empty directory - this can go very wrong")
            else()
                execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory "${workdir}")
            endif()
        endif()
        execute_process(COMMAND "${CMAKE_COMMAND}" -E touch "${_mark}")
        message(STATUS "finished unpacking ${file} ----> ${dir}")
    endif()
endfunction()

#------------------------------------------------------------------------------

set(cmk_repo_url https://raw.githubusercontent.com/biojppm/cmake/master)
download_file(${cmk_repo_url}/ExternalProjectUtils.cmake ${CMAKE_CURRENT_BINARY_DIR})
download_file(${cmk_repo_url}/PrintVar.cmake ${CMAKE_CURRENT_BINARY_DIR})
download_file(${cmk_repo_url}/PatchUtils.cmake ${CMAKE_CURRENT_BINARY_DIR})

#------------------------------------------------------------------------------

include(ExternalProject)
include(${CMAKE_CURRENT_BINARY_DIR}/PrintVar.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/PatchUtils.cmake)
# this allows forwarding our compilation environment to each of the subprojects.
include(${CMAKE_CURRENT_BINARY_DIR}/ExternalProjectUtils.cmake)
