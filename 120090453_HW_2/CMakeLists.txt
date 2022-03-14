cmake_minimum_required( VERSION 2.8 )
project( HW1 )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
aux_source_directory(. ALL_SRCS)

add_executable( compile_run.sh ${ALL_SRCS})
target_link_libraries( compile_run.sh ${OpenCV_LIBS} )