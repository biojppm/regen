
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
function(regen_setup wdir generated_headers generated_sources)
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
        if(ghdr OR gsrc)
            message(STATUS " ... regen: ${r}  ---->  ${ghdr} ${gsrc}")
            set(fghdr "${wdir}/${ghdr}")
            set(fgsrc "${wdir}/${gsrc}")
            # save the names
            list(APPEND hdrs ${fghdr})
            list(APPEND srcs ${fgsrc})
            #
            add_custom_command(OUTPUT ${fghdr} ${fgsrc}
                COMMAND pwd
                COMMAND ${REGEN_EXEC} --gen-code ${REGEN_ARGS} ${r}
                WORKING_DIRECTORY ${wdir}
                DEPENDS ${r} ${REGEN_FILE}
                COMMENT "regen: ${r}  ---->  ${ghdr} ${gsrc}")
        else()
            message(STATUS " ... regen: ${r}")
        endif()
    endforeach()
    set(${generated_headers} ${hdrs} PARENT_SCOPE)
    set(${generated_sources} ${srcs} PARENT_SCOPE)
endfunction()
