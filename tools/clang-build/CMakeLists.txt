cmake_minimum_required(VERSION 3.2)

project(clang-build)

option(CLANGBUILD_USE_IWYU "" ON)

set(IDIR ${CMAKE_INSTALL_PREFIX}) # install dir
set(SDIR ${CMAKE_SOURCE_DIR}) # source dir
set(BDIR ${CMAKE_BINARY_DIR}/bindirs) # common dir to place binary dirs of each proj

if(MSVC)
    set(disabled_warnings
        /wd4018   # '<': signed/unsigned mismatch
        /wd4101   # 't': unreferenced local variable
        /wd4244   # conversion from 'type1' to 'type2', possible loss of data
        /wd4267   # conversion from 'type1' to 'type2', possible loss of data
        /wd4305   # 'initializing': truncation from 'double' to 'float'
        /wd4309   # 'argument': truncation of constant value
        /wd4312   # 'type cast': conversion from 'int' to 'unsigned char *' of greater size
        /wd4800   # 'int': forcing value to bool 'true' or 'false'
        /wd4996   # This function or variable may be unsafe
        )

    foreach(dw ${disabled_warnings})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${dw}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${dw}")
    endforeach()
endif()

#------------------------------------------------------------------------------

function(download_file url dest)
    if(NOT EXISTS ${dest})
        message(STATUS "downloading ${url}...")
        file(DOWNLOAD ${url} ${dest})
        message(STATUS "finished downloading: ${dest}")
    endif()
endfunction()

function(unpack_to file dir)
    get_filename_component(basename ${file} NAME)
    set(_mark ${CMAKE_CURRENT_BINARY_DIR}/${basename}.done)
    if(NOT EXISTS ${_mark})
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

download_file(
    https://raw.githubusercontent.com/biojppm/cmake/master/ExternalProjectUtils.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ExternalProjectUtils.cmake
    )

download_file(
    https://raw.githubusercontent.com/biojppm/cmake/master/PrintVar.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/PrintVar.cmake
    )

#------------------------------------------------------------------------------

include(ExternalProject)
# this allows forwarding our compilation environment to each of the subprojects.
include(${CMAKE_CURRENT_BINARY_DIR}/ExternalProjectUtils.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/PrintVar.cmake)
ExternalProject_GetFwdArgs(CMK_ARGS)

list(APPEND CMK_ARGS -DCMAKE_PREFIX_PATH=${IDIR})
list(APPEND CMK_ARGS -DCMAKE_INCLUDE_PATH=${IDIR}/include)
list(APPEND CMK_ARGS -DCMAKE_LIBRARY_PATH=${IDIR}/lib)

#------------------------------------------------------------------------------

if(NOT CLANG_VERSION)
    set(CLANG_VERSION 3.9.1)
endif()

foreach(ver ${CLANG_VERSION})

    download_file(
        http://releases.llvm.org/${ver}/llvm-${ver}.src.tar.xz
        ${SDIR}/llvm-${ver}.src.tar.xz
        )

    download_file(
        http://releases.llvm.org/${ver}/cfe-${ver}.src.tar.xz
        ${SDIR}/cfe-${ver}.src.tar.xz
        )

    download_file(
        http://releases.llvm.org/${ver}/clang-tools-extra-${ver}.src.tar.xz
        ${SDIR}/clang-tools-extra-${ver}.src.tar.xz
        )

    unpack_to(
        ${SDIR}/llvm-${ver}.src.tar.xz
        ${SDIR}/llvm-${ver}
        )

    unpack_to(
        ${SDIR}/cfe-${ver}.src.tar.xz
        ${SDIR}/llvm-${ver}/tools/clang
        )

    unpack_to(
        ${SDIR}/clang-tools-extra-${ver}.src.tar.xz
        ${SDIR}/llvm-${ver}/tools/clang/tools/extra
        )

    ExternalProject_Add(llvm-${ver}
        PREFIX llvm-${ver}
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${SDIR}/llvm-${ver}
        BINARY_DIR ${BDIR}/llvm-${ver}
        INSTALL_DIR ${IDIR}/llvm-${ver}
        CMAKE_ARGS ${CMK_ARGS}
        )

endforeach()

#------------------------------------------------------------------------------

return()  # stop here

ExternalProject_Add(<name>    # Name for custom target
    [DEPENDS projects...]       # Targets on which the project depends
    [PREFIX dir]                # Root dir for entire project
    [LIST_SEPARATOR sep]        # Sep to be replaced by ; in cmd lines
    [TMP_DIR dir]               # Directory to store temporary files
    [STAMP_DIR dir]             # Directory to store step timestamps
   #--Download step--------------
    [DOWNLOAD_DIR dir]          # Directory to store downloaded files
    [DOWNLOAD_COMMAND cmd...]   # Command to download source tree
    [CVS_REPOSITORY cvsroot]    # CVSROOT of CVS repository
    [CVS_MODULE mod]            # Module to checkout from CVS repo
    [CVS_TAG tag]               # Tag to checkout from CVS repo
    [SVN_REPOSITORY url]        # URL of Subversion repo
    [SVN_REVISION rev]          # Revision to checkout from Subversion repo
    [SVN_USERNAME john ]        # Username for Subversion checkout and update
    [SVN_PASSWORD doe ]         # Password for Subversion checkout and update
    [SVN_TRUST_CERT 1 ]         # Trust the Subversion server site certificate
    [GIT_REPOSITORY url]        # URL of git repo
    [GIT_TAG tag]               # Git branch name, commit id or tag
    [URL /.../src.tgz]          # Full path or URL of source
    [URL_MD5 md5]               # MD5 checksum of file at URL
    [TIMEOUT seconds]           # Time allowed for file download operations
   #--Update/Patch step----------
    [UPDATE_COMMAND cmd...]     # Source work-tree update command
    [PATCH_COMMAND cmd...]      # Command to patch downloaded source
   #--Configure step-------------
    [SOURCE_DIR dir]            # Source dir to be used for build
    [CONFIGURE_COMMAND cmd...]  # Build tree configuration command
    [CMAKE_COMMAND /.../cmake]  # Specify alternative cmake executable
    [CMAKE_GENERATOR gen]       # Specify generator for native build
    [CMAKE_ARGS args...]        # Arguments to CMake command line
    [CMAKE_CACHE_ARGS args...]  # Initial cache arguments, of the form -Dvar:string=on
   #--Build step-----------------
    [BINARY_DIR dir]            # Specify build dir location
    [BUILD_COMMAND cmd...]      # Command to drive the native build
    [BUILD_IN_SOURCE 1]         # Use source dir for build dir
   #--Install step---------------
    [INSTALL_DIR dir]           # Installation prefix
    [INSTALL_COMMAND cmd...]    # Command to drive install after build
   #--Test step------------------
    [TEST_BEFORE_INSTALL 1]     # Add test step executed before install step
    [TEST_AFTER_INSTALL 1]      # Add test step executed after install step
    [TEST_COMMAND cmd...]       # Command to drive test
   #--Output logging-------------
    [LOG_DOWNLOAD 1]            # Wrap download in script to log output
    [LOG_UPDATE 1]              # Wrap update in script to log output
    [LOG_CONFIGURE 1]           # Wrap configure in script to log output
    [LOG_BUILD 1]               # Wrap build in script to log output
    [LOG_TEST 1]                # Wrap test in script to log output
    [LOG_INSTALL 1]             # Wrap install in script to log output
   #--Custom targets-------------
    [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
)
