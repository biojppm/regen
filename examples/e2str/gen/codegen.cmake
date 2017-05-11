set(CODEGEN "python3 codegen.py" CACHE STRING "invocation of a codegen executable")

function(capture_output output_var)
    set(cmd)
    foreach(a ${ARGN})
        set(cmd "${cmd} ${a}")
    endforeach()
    message(STATUS "executing command:${cmd}")
    execute_process(COMMAND ${ARGN}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE out
        ERROR_VARIABLE err
        RESULT_VARIABLE res)
    string(REPLACE "\n" "" out2 "${out}")
    #message(STATUS "      res=${res}  out=${out2} err=${err}")
    set(${output_var} ${out2} PARENT_SCOPE)
    if(${res})
        message(FATAL_ERROR "could not execute command: ${cmd}")
    endif()
endfunction(capture_output)


function(setup_codegen_genfile genroot headers sources)
    set(hdrs)
    set(srcs)
    if(genroot)
        set(genroot "${genroot}/")
    endif()
    foreach(r ${ARGN})
        # find the files generated from this file
        capture_output(ghdr ${CODEGEN} --show-out-hdr ${r})
        capture_output(gsrc ${CODEGEN} --show-out-src ${r})
        # if there are any...
        if(ghdr OR gsrc)
        #message(STATUS "....codegen: ${r}  ---->  ${ghdr} ${gsrc}")
            set(fghdr "${genroot}${ghdr}")
            set(fgsrc "${genroot}${gsrc}")
            # save the names
            list(APPEND hdrs ${fghdr})
            list(APPEND srcs ${fgsrc})
            add_custom_command(
                OUTPUT ${fghdr} ${fgsrc}
                COMMAND ${PYTHON3} codegen.py --gen-code ${r}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                DEPENDS ${r} ${PROJECT_SOURCE_DIR}/codegen.py
                COMMENT "codegen: ${r}  ---->  ${ghdr} ${gsrc}")
        endif()
    endforeach()
    set(${headers} ${hdrs} PARENT_SCOPE)
    set(${sources} ${srcs} PARENT_SCOPE)
endfunction()

function(setup_codegen_samefile genroot headers sources)
    set(hdrs)
    set(srcs)
    if(genroot)
        set(genroot "${genroot}/")
    endif()
    foreach(r ${ARGN})
        # find the files generated from this file
        capture_output(ghdr ${CODEGEN} --show-out-hdr ${r})
        capture_output(gsrc ${CODEGEN} --show-out-src ${r})
        # if there are any...
        if(ghdr OR gsrc)
        #message(STATUS "....codegen: ${r}  ---->  ${ghdr} ${gsrc}")
            set(fghdr "${genroot}${ghdr}")
            set(fgsrc "${genroot}${gsrc}")
            # save the names
            list(APPEND hdrs ${fghdr})
            list(APPEND srcs ${fgsrc})
            add_custom_command(
                OUTPUT ${fghdr} ${fgsrc}
                COMMAND ${PYTHON3} codegen.py --gen-code ${r}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                DEPENDS ${r} ${PROJECT_SOURCE_DIR}/codegen.py
                COMMENT "codegen: ${r}  ---->  ${ghdr} ${gsrc}")
        endif()
    endforeach()
    set(${headers} ${hdrs} PARENT_SCOPE)
    set(${sources} ${srcs} PARENT_SCOPE)
endfunction()

