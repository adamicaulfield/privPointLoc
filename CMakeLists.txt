cmake_minimum_required(VERSION 3.15)
project(privPointLoc LANGUAGES CXX)

set(CMAKE_FIND_FRAMEWORK LAST)
set(CMAKE_FIND_APPBUNDLE LAST)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 17)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build/private/local_config.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/build/private/local_config.cmake)
endif()
#
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(Project_Name privPointLoc)

# Dependency paths
set(DEPS_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/deps/include)
set(DEPS_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/deps/lib)
set(DEPS_SOURCE_PATH ${PROJECT_SOURCE_DIR}/deps/src)
set(DEPS_BINARY_PATH ${PROJECT_SOURCE_DIR}/deps/bin)

# Output configurations
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(TEST_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/tmp)

# Input configuration
set(SOURCES_DIR ${PROJECT_SOURCE_DIR}/src)
set(SOURCE_FILES
        ${SOURCES_DIR}/Encryptor.cpp
        ${SOURCES_DIR}/FileSystem.cpp
        ${SOURCES_DIR}/Tree.cpp
        ${SOURCES_DIR}/Segment.cpp
        ${SOURCES_DIR}/Node.cpp
        ${SOURCES_DIR}/PrivPointUtil.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE CXX)

include_directories(${DEPS_INCLUDE_PATH} src include /usr/local/include)

link_directories(${DEPS_LIBRARY_PATH} /usr/local/lib)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

#OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)
#if(USE_OpenMP)
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
#endif()

add_custom_target(build-time-make-directory ALL
COMMAND ${CMAKE_COMMAND} -E make_directory "${TEST_OUTPUT_PATH}/keys"
COMMAND ${CMAKE_COMMAND} -E make_directory "${TEST_OUTPUT_PATH}/records")

