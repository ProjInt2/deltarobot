cmake_minimum_required(VERSION 3.2)
project(libdeltarobot LANGUAGES C CXX VERSION 0.1) 
set (CMAKE_CXX_STANDARD_REQUIRED 11)

add_library( deltarobot STATIC src/deltarobot.cpp src/protocol.c src/protocol_pc_utils.cpp )
target_include_directories( deltarobot PUBLIC ${CMAKE_SOURCE_DIR}/include )


export( TARGETS deltarobot FILE ${CMAKE_CURRENT_BINARY_DIR}/deltarobot.cmake )


# TESTS

add_executable( send_opcode test/send_opcode.c )
add_executable( cross_comm test/cross_comm.cpp )
