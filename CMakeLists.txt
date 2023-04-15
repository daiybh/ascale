cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




project ("Fee")



add_executable(Fee "a.cpp" "uyvy8bit_toAny.h" "pixelHeader.h" "yuv10packing_toAny.h")