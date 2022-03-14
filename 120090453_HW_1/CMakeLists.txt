cmake_minimum_required( VERSION 2.8 )
project( HW1 )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(. ALL_SRCS)
add_subdirectory(src)
add_executable( compile_run.sh ${ALL_SRCS})
target_link_libraries( compile_run.sh lib)
target_link_libraries( compile_run.sh ${OpenCV_LIBS} )