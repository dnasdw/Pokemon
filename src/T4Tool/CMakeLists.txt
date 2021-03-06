AUTO_FILES("." "src" "\\.(cpp|h)$")
AUTO_FILES("${ROOT_SOURCE_DIR}/dep/libsundaowen" "src" "\\.(cpp|h)$")
include_directories(${DEP_INCLUDE_DIR})
link_directories(${DEP_LIBRARY_DIR})
add_definitions(-DSDW_MAIN)
ADD_EXE(T4Tool "${src}")
if(WIN32)
  if(MSVC)
    target_link_libraries(T4Tool libpng16_static zlibstatic PVRTexLib)
    set_target_properties(T4Tool PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:LIBCMT")
  else()
    target_link_libraries(T4Tool png16 z)
  endif()
else()
  target_link_libraries(T4Tool png16 z PVRTexLib pthread)
  if(CYGWIN)
    target_link_libraries(T4Tool iconv)
  endif()
endif()
GET_CURRENT_DEP_LIBRARY_PREFIX("${ROOT_SOURCE_DIR}/dep/PVRTexTool/Library")
if(WIN32)
  add_custom_command(TARGET T4Tool POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ROOT_SOURCE_DIR}/dep/PVRTexTool/Library/${CURRENT_DEP_LIBRARY_PREFIX}/PVRTexLib.dll" $<TARGET_FILE_DIR:T4Tool>)
elseif(APPLE)
  add_custom_command(TARGET T4Tool POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ROOT_SOURCE_DIR}/dep/PVRTexTool/Library/${CURRENT_DEP_LIBRARY_PREFIX}/libPVRTexLib.dylib" $<TARGET_FILE_DIR:T4Tool> COMMAND install_name_tool -change @executable_path/../lib/libPVRTexLib.dylib @executable_path/libPVRTexLib.dylib $<TARGET_FILE:T4Tool>)
else()
  add_custom_command(TARGET T4Tool POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ROOT_SOURCE_DIR}/dep/PVRTexTool/Library/${CURRENT_DEP_LIBRARY_PREFIX}/libPVRTexLib.so" $<TARGET_FILE_DIR:T4Tool>)
endif()
install(TARGETS T4Tool DESTINATION bin)
if(WIN32)
  install(FILES $<TARGET_FILE_DIR:T4Tool>/PVRTexLib.dll DESTINATION bin)
elseif(APPLE)
  install(FILES $<TARGET_FILE_DIR:T4Tool>/libPVRTexLib.dylib DESTINATION bin)
else()
  install(FILES $<TARGET_FILE_DIR:T4Tool>/libPVRTexLib.so DESTINATION bin)
endif()
