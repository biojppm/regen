cmake_minimum_required(VERSION 3.2)

project(clang-build)

option(CLANGBUILD_USE_IWYU "" ON)

set(IDIR ${CMAKE_INSTALL_PREFIX}) # install dir
set(SDIR ${CMAKE_SOURCE_DIR}/src) # source dir
set(PDIR ${CMAKE_SOURCE_DIR}/src/pack) # packs dir
set(BDIR ${CMAKE_BINARY_DIR}/bindirs) # common dir to place binary dirs of each proj

#------------------------------------------------------------------------------
include(util.cmake)
ExternalProject_GetFwdArgs(CMK_ARGS EXCLUDE CMAKE_INSTALL_PREFIX)

#------------------------------------------------------------------------------

if(NOT CLANG_VERSION)
    set(CLANG_VERSION 3.9.1)
endif()

foreach(ver ${CLANG_VERSION})
    if(NOT EXISTS ${SDIR}/llvm-${ver})
        download_file(http://releases.llvm.org/${ver}/llvm-${ver}.src.tar.xz ${PDIR})
        download_file(http://releases.llvm.org/${ver}/cfe-${ver}.src.tar.xz ${PDIR})
        download_file(http://releases.llvm.org/${ver}/clang-tools-extra-${ver}.src.tar.xz ${PDIR})

        unpack_to(${PDIR}/llvm-${ver}.src.tar.xz ${SDIR}/llvm-${ver})
        unpack_to(${PDIR}/cfe-${ver}.src.tar.xz ${SDIR}/llvm-${ver}/tools/clang)
        unpack_to(${PDIR}/clang-tools-extra-${ver}.src.tar.xz ${SDIR}/llvm-${ver}/tools/clang/tools/extra)
    endif()
endforeach()

#------------------------------------------------------------------------------
# apply needed patches

# clang 3.8.1 needs to be patched for VS2015 Update 3.
# the patch is downloaded and adapted from https://reviews.llvm.org/D20492
# clang will also need -fms-compatibility-version=19
# see also https://www.reddit.com/r/cpp/comments/50x2ee/how_to_get_clang_to_work/
if(MSVC)
    if(EXISTS ${SDIR}/llvm-3.8.1)
        set(mark ${SDIR}/llvm-3.8.1/D20492.diff.patch.done)
        apply_patch(${CMAKE_CURRENT_SOURCE_DIR}/D20492.diff ${SDIR}/llvm-3.8.1/tools/clang ${mark})
    endif()
endif()

#------------------------------------------------------------------------------

foreach(ver ${CLANG_VERSION})
    ExternalProject_Add(llvm-${ver}
        PREFIX llvm-${ver}
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${SDIR}/llvm-${ver}
        BINARY_DIR ${BDIR}/llvm-${ver}
        #INSTALL_DIR ${IDIR}/llvm-${ver}
        CMAKE_ARGS "${CMK_ARGS};-DCMAKE_INSTALL_PREFIX=${IDIR}/llvm-${ver}"
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
