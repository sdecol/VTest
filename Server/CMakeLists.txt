cmake_minimum_required(VERSION 3.25)
project(Server)

set(CMAKE_CXX_STANDARD 20)

#SERVER SOURCES FILES

set (SERVER_SRC_FILES

${CMAKE_SOURCE_DIR}/src/Main/*.cpp
${CMAKE_SOURCE_DIR}/src/ServerApplication/*.hpp
${CMAKE_SOURCE_DIR}/src/ServerApplication/*.cpp

)

#INCLUDE DIRECTORIES

include_directories(${CMAKE_SOURCE_DIR}/src)

#GROUPING SOURCE FILES IN A TREE

file( GLOB SERVER_SRC LIST_DIRECTORIES FALSE ${SERVER_SRC_FILES})
source_group( TREE ${CMAKE_SOURCE_DIR}/src PREFIX Server FILES ${SERVER_SRC})

#GROUPING COMMON SRC FILES IN A TREE

include(${CMAKE_SOURCE_DIR}/../Common/CMakeLists.txt)
file( GLOB COMMON_SRC LIST_DIRECTORIES FALSE ${COMMON_SOURCE_FILES})
source_group( TREE ${CMAKE_SOURCE_DIR}/../Common PREFIX Common FILES ${COMMON_SRC})

#ADD EXECUTABLE

add_executable (Server ${SERVER_SRC} ${COMMON_SRC})
target_link_libraries(Server PRIVATE pthread)


