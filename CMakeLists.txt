cmake_minimum_required(VERSION 3.11)

project(dpabc VERSION 0.0.1 DESCRIPTION "Iplementation of dP-ABC (PS-MS) in C, using wrapper library")

# C11
set(CMAKE_C_STANDARD 11)

# Fix for sh.exe in path
set(DCMAKE_SH="CMAKE_SH-NOTFOUND")

# Fix for cmocka in windows
if(WIN32)
    set(CMOCKA_PATH "C:/MinGW/bin/cmocka/bin/cmocka.dll")
endif(WIN32)

# Set default value for wrapper library instantiation 
if(NOT WRAPPER_INSTANTIATION)
    set(WRAPPER_INSTANTIATION "pfec_Miracl_Bls381_64")
endif()

# Set folder paths
SET(SRC_PATH_PABC "${PROJECT_SOURCE_DIR}/src")
SET(HEADER_PATH_PABC "${PROJECT_SOURCE_DIR}/include")
SET(LIB_PATH_PABC "${PROJECT_SOURCE_DIR}/lib")
SET(TEST_PATH_PABC "${PROJECT_SOURCE_DIR}/test")

# Add libraries' subdirectories
add_subdirectory("${LIB_PATH_PABC}/pfecCwrapper")

# Includes (must be after add_subdirectories for libraries. Alternatively, use target_include_directories)
include_directories(${HEADER_PATH_PABC})

#Library with dpabc functionality, wrapper instantiation established by user (or default)
add_library(dpabc_psms STATIC 
        "${SRC_PATH_PABC}/PSMS/Dpabc_utils.h"
        "${SRC_PATH_PABC}/PSMS/types_impl.h"
        "${SRC_PATH_PABC}/PSMS/Dpabc_utils.c"
        "${SRC_PATH_PABC}/PSMS/Dpabc_types.c"
        "${SRC_PATH_PABC}/PSMS/Dpabc.c"
        )

target_include_directories(dpabc_psms PUBLIC ${HEADER_PATH_PABC})

target_link_libraries(dpabc_psms ${WRAPPER_INSTANTIATION})

# Example binary
add_executable(dpabc_example 
        "${SRC_PATH_PABC}/example/main.c")
target_link_libraries(dpabc_example dpabc_psms)

# Testing
enable_testing()


add_cmocka_test(pabcTestCmocka 
                SOURCES "${TEST_PATH_PABC}/main.c" 
                LINK_LIBRARIES dpabc_psms cmocka)
                
# Modifying output folders for libs/binaries
set_target_properties(dpabc_example dpabc_psms
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib"
LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib"
RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin"
)

# Fix for cmocka in windows
if(WIN32)
        file(COPY ${CMOCKA_PATH} DESTINATION ${PROJECT_BINARY_DIR})
endif(WIN32)