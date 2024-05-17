cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




project ("Fee")
#[[
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)
]]

#add_executable(${PROJECT_NAME} "a.cpp" "uyvy8bit_toAny.h" "pixelHeader.h" "yuv10packing_toAny.h")
add_executable(yuvConv "source/a.cpp")
#[[
add_executable(${PROJECT_NAME} "source/main_sdl.cpp")

target_link_directories(${PROJECT_NAME} PRIVATE "source/")
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2-static)

# SDL image to make a surface (aka what we'll render)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image-static)

# SDL ttf so we can display hello world!
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf-static)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin)
MESSAGE(STATUS CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
if(WIN32)
install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX} OPTIONAL)
endif(WIN32)
]]