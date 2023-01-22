if (WORKING_FROM_TOP_LEVEL)
else(WORKING_FROM_TOP_LEVEL)
    message(FATAL_ERROR "Not working from top level! If you are including this in your own project then please set this variable in your CMakeLists.txt.")
endif(WORKING_FROM_TOP_LEVEL)

file(GLOB_RECURSE sourceFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.m
)

#file(GLOB resFiles
#   ${SAKURA_SRC_DIR}/Sakura/SakuraShaders/*.cg
#)

#set_source_files_properties(
#   ${resFiles}
#   PROPERTIES
#   MACOSX_PACKAGE_LOCATION Resources
#)

add_executable(SakuraTestApp WIN32 MACOSX_BUNDLE ${sourceFiles} ${resFiles})

if (MSVC)
    #Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(SakuraTestApp PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

#Copy any required resources to the data directory.
file(GLOB_RECURSE sakuraShaders
    ${SAKURA_SRC_DIR}/Sakura/SakuraShaders/${RENDER_API_DIR}/${SHADER_APT_DIR}/*.*
)

foreach(shader ${sakuraShaders})
    add_custom_command(TARGET SakuraTestApp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${shader} ${CMAKE_SOURCE_DIR}/Data/)
endforeach(shader)

#Copy the data into the bundle.
if (APPLE)
#  set(sakuratestapp_resources 
#   ${CMAKE_SOURCE_DIR}/Data/*
#  )
#  add_custom_command(TARGET SakuraTestApp 
#                    POST_BUILD 
#                    COMMAND ${CMAKE_COMMAND} -E make_directory \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME}/Contents/Resources
#  )
#  add_custom_command(TARGET SakuraTestApp 
#                    POST_BUILD 
#                    COMMAND /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -resolve-src-symlinks ${sakuratestapp_resources} \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME}/Contents/Resources
#  )
endif()

target_link_libraries(SakuraTestApp Sakura SakuraMaths SakuraNet SakuraThread)
