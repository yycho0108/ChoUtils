cmake_minimum_required(VERSION 3.12)

add_library(core
    src/random.cpp
    src/math.cpp
    )

target_include_directories(core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

add_library(${PROJECT_NAME}::core ALIAS core)

export (TARGETS core NAMESPACE ${PROJECT_NAME}:: FILE ${PROJECT_NAME}Targets.cmake)

generate_export_header(core)
install(
    TARGETS core
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
