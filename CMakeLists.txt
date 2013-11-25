cmake_minimum_required(VERSION 2.8)
project( Assignment1 )


find_package( OpenCV REQUIRED )

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

add_executable( quantize quantize.cpp )
target_link_libraries( quantize ${OpenCV_LIBS} )

add_executable( interpolation interpolation.cpp )
target_link_libraries( interpolation ${OpenCV_LIBS} )

add_executable( histogram histogram.cpp )
target_link_libraries( histogram ${OpenCV_LIBS} )
