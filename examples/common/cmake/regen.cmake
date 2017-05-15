
#------------------------------------------------------------------------------
function(_regen_capture_output output_var wdir)
    set(cmd)
    #message(STATUS "executing command (at ${wdir}): ${ARGN}")
    execute_process(COMMAND ${ARGN}
        WORKING_DIRECTORY ${wdir}
        OUTPUT_VARIABLE out
        ERROR_VARIABLE err
        RESULT_VARIABLE res)
    string(REPLACE "\n" "" out2 "${out}")
    if(NOT "${res}" STREQUAL "0")
        message(FATAL_ERROR "command failed (status=${res}): ${ARGN}. err=\n${err}")
    endif()
    set(${output_var} ${out2} PARENT_SCOPE)
endfunction()


#------------------------------------------------------------------------------
function(regen_setup wdir generated_headers generated_sources generated_targets)
    if(NOT TARGET regen)
        add_custom_target(regen)
    endif()
    message(STATUS "regen: checking dependencies...")
    set(REGEN_FILE ${wdir}/regen.py)
    set(REGEN_EXEC python3 ${REGEN_FILE})
    set(REGEN_ARGS --clang-args "-std=c++11 -I ${wdir}")
    set(hdrs)
    set(srcs)
    foreach(r ${ARGN}) # for each file...
        # find the files generated from this file
        _regen_capture_output(ghdr "${wdir}" ${REGEN_EXEC} --show-hdr ${REGEN_ARGS} ${r})
        _regen_capture_output(gsrc "${wdir}" ${REGEN_EXEC} --show-src ${REGEN_ARGS} ${r})
        # if there are any generated sources...
        if(NOT (ghdr OR gsrc))
            message(STATUS " ... regen: ${r}")
        else()
            message(STATUS " ... regen: ${r}  ---->  ${ghdr} ${gsrc}")
            set(fghdr "${wdir}/${ghdr}")
            set(fgsrc "${wdir}/${gsrc}")
            # since some writers can write into the source file and cmake
            # will see this as a circular dependency, always generate an
            # output file; only mark the output files as such if they're
            # not the source file
            set(done_file "${CMAKE_CURRENT_BINARY_DIR}/${r}.regen.done")
            set(output_files "${done_file}")
            if(ghdr AND (NOT "${ghdr}" STREQUAL "${r}"))
                list(APPEND output_files ${fghdr})
            endif()
            if(gsrc AND (NOT "${gsrc}" STREQUAL "${r}"))
                list(APPEND output_files ${fgsrc})
            endif()
            #message(STATUS "aqui 2 done_file=${done_file}")
            #message(STATUS "aqui 3 output_files=${output_files}")
            # add a custom command to run regen
            add_custom_command(OUTPUT ${output_files}
                DEPENDS "${r}" "${REGEN_FILE}" "${CMAKE_CURRENT_LIST_FILE}"
                COMMAND ${REGEN_EXEC} --gen-code ${REGEN_ARGS} ${r}
                COMMAND ${CMAKE_COMMAND} -E touch "${done_file}"
                WORKING_DIRECTORY ${wdir}
                COMMENT "regen@${CMAKE_CURRENT_SOURCE_DIR}: ${r}  ---->  ${ghdr} ${gsrc}")
            # see http://stackoverflow.com/questions/12913077/cmake-add-dependency-to-add-custom-command-dynamically
            # cannot add a dependency which is the OUTPUT of a custom command
            # but add_custom_target() allows non-existing dependencies in its
            # DEPENDS clause. So we do the following:
            add_custom_target(${r}-regen-target DEPENDS ${done_file})
            add_dependencies(regen ${r}-regen-target)
            # save the names
            list(APPEND hdrs ${fghdr})
            list(APPEND srcs ${fgsrc})
            list(APPEND tgts ${r}-regen-target)
        endif()
    endforeach()
    set(${generated_headers} ${hdrs} PARENT_SCOPE)
    set(${generated_sources} ${srcs} PARENT_SCOPE)
    set(${generated_targets} ${tgts} PARENT_SCOPE)
endfunction()
