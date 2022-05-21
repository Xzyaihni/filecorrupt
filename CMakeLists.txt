cmake_minimum_required(VERSION 3.13)
project(filecorrupt)
set(CMAKE_CXX_STANDARD 20)

option(Y_DEBUG "build in debug mode" "OFF")
option(Y_SANITIZE "build with address sanitizer" "OFF")


set(SOURCE_FILES main.cpp
corrupt.cpp)

if(${Y_DEBUG})
	set(CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(${PROJECT_SOURCE_DIR})

find_package(wxWidgets REQUIRED COMPONENTS net core base)

include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/${SOURCE_FILES})



if(${Y_DEBUG})
	target_link_libraries(${PROJECT_NAME} -O1 -pg -Wall)
	
	if(${Y_SANITIZE})
		target_link_libraries(${PROJECT_NAME} -fsanitize=address)
	endif()
else()
	target_link_libraries(${PROJECT_NAME} -O3)
endif()

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
