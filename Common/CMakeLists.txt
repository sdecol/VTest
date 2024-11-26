project(VTestCommon)

#COMMON SRC FILES USED BY BOTH CLIENT AND SERVER

set (COMMON_SOURCE_FILES

		${CMAKE_CURRENT_LIST_DIR}/src/*.hpp
		${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/nlhomann/*.hpp
	${CMAKE_CURRENT_LIST_DIR}/src/httplib/*.h
	${CMAKE_CURRENT_LIST_DIR}/src/ArgumentParser/*.hpp
	${CMAKE_CURRENT_LIST_DIR}/src/ArgumentParser/*.cpp

)

#INCLUDE DIRS

include_directories(

        ${CMAKE_CURRENT_LIST_DIR}/src
)

