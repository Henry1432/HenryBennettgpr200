#Transformations example

file(
 GLOB_RECURSE ASSIGNMENT8_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENT8_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignment7_lighting's asset folder to bin when it is built
add_custom_target(copyAssetsA8 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENT8_INC} DESTINATION include/final_v1_Billboarding)
add_executable(final_v1_Billboarding ${ASSIGNMENT8_SRC} ${ASSIGNMENT8_INC} ${ASSIGNMENT8_ASSETS})
target_link_libraries(final_v1_Billboarding PUBLIC core IMGUI)
target_include_directories(final_v1_Billboarding PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when final_v1_Billboarding is built
add_dependencies(final_v1_Billboarding copyAssetsA8)