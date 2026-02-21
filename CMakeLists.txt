cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(cue VERSION 1.0.0)

file(GLOB_RECURSE SOURCES
	src/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# unity build unless disabled
# option(CUE_UNITY_BUILD "Enable unity build" ON)
# if (CUE_UNITY_BUILD)
#     set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE 32)
# endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE GEODE_MOD_ID="_cue")
target_compile_definitions(${PROJECT_NAME} PRIVATE CUE_VERSION="${PROJECT_VERSION}")

target_link_libraries(${PROJECT_NAME} geode-sdk)

target_include_directories(${PROJECT_NAME} PUBLIC include)
