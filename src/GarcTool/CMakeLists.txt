AUTO_FILES("." "src" "\\.(cpp|h)$")
AUTO_FILES("${ROOT_SOURCE_DIR}/dep/libsundaowen" "src" "\\.(cpp|h)$")
include_directories(${DEP_INCLUDE_DIR})
link_directories(${DEP_LIBRARY_DIR})
add_definitions(-DSDW_MAIN)
ADD_EXE(GarcTool "${src}")
if(CYGWIN)
  target_link_libraries(GarcTool iconv)
endif()
install(TARGETS GarcTool DESTINATION bin)
