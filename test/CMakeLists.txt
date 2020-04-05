cmake_minimum_required(VERSION 2.8)
project(choutil-test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# message("$ENV{HOME}/Repos/Ravel/ChoUtils/install/")
find_package(ChoUtil REQUIRED PATHS "$ENV{HOME}/Repos/Ravel/ChoUtils/install/")

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
   message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${ChoUtil_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ChoUtil::core ChoUtil::util)
