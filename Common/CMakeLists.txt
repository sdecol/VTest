project(VTestCommon)

#COMMON SRC FILES USED BY BOTH CLIENT AND SERVER

set (COMMON_SOURCE_FILES

	${CMAKE_CURRENT_LIST_DIR}/nlhomann/*.hpp

)

#INCLUDE DIRS

include_directories(

        ${CMAKE_CURRENT_LIST_DIR}
)

