set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(regen-tool
  RegenTool.cpp
  )
#target_include_directories(regen-tool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/regenxx)

#add_subdirectory(regenxx/rapidyaml)

target_link_libraries(regen-tool
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangTooling
#  ryml
  )

install(TARGETS regen-tool
  RUNTIME DESTINATION bin)
