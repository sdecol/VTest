cmake_minimum_required(VERSION 3.25)
project(Client)

set(CMAKE_CXX_STANDARD 20)

#CLIENT SOURCES FILES

set (CLIENT_SRC_FILES

${CMAKE_SOURCE_DIR}/src/Main/*.cpp
${CMAKE_SOURCE_DIR}/src/ClientApplication/*.hpp
${CMAKE_SOURCE_DIR}/src/ClientApplication/*.cpp
${CMAKE_SOURCE_DIR}/src/ClientApplication/ClientInput/*.hpp
${CMAKE_SOURCE_DIR}/src/ClientApplication/ClientInput/*.cpp

)

#INCLUDE DIRECTORIES

include_directories(${CMAKE_SOURCE_DIR}/src)

#GROUPING SOURCE FILES IN A TREE

file( GLOB CLIENT_SRC LIST_DIRECTORIES FALSE ${CLIENT_SRC_FILES})
source_group( TREE ${CMAKE_SOURCE_DIR}/src PREFIX Client FILES ${CLIENT_SRC})

#GROUPING COMMON SRC FILES IN A TREE

include(${CMAKE_SOURCE_DIR}/../Common/CMakeLists.txt)
file( GLOB COMMON_SRC LIST_DIRECTORIES FALSE ${COMMON_SOURCE_FILES})
source_group( TREE ${CMAKE_SOURCE_DIR}/../Common PREFIX Common FILES ${COMMON_SRC})

#ADD EXECUTABLE

add_executable (Client ${CLIENT_SRC} ${COMMON_SRC})
target_link_libraries(Client PRIVATE pthread)