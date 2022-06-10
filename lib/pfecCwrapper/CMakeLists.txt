cmake_minimum_required(VERSION 3.11)

project(pfeclib VERSION 0.0.1 DESCRIPTION "Wrapper library for pairing-friendly EC arithmetic")

# C11
set(CMAKE_C_STANDARD 11)

# Fix for sh.exe in path
set(DCMAKE_SH="CMAKE_SH-NOTFOUND")

#Fix for cmocka in Windows (need to adjust path to your installation)
if(WIN32 AND NOT CMOCKA_PATH)
    set(CMOCKA_PATH "C:/MinGW/bin/cmocka/bin/cmocka.dll")
endif(WIN32 AND NOT CMOCKA_PATH)

# Set folder paths
SET(SRC_PATH_WRAPPER "${PROJECT_SOURCE_DIR}/src")
SET(HEADER_PATH_WRAPPER "${PROJECT_SOURCE_DIR}/include")
SET(LIB_PATH_WRAPPER "${PROJECT_SOURCE_DIR}/lib")
SET(TEST_PATH_WRAPPER "${PROJECT_SOURCE_DIR}/test")

# Add libraries' subdirectories
add_subdirectory("${LIB_PATH_WRAPPER}/Miracl_BLS12381_32b")
add_subdirectory("${LIB_PATH_WRAPPER}/Miracl_BLS12381_64b")

# Includes (must be after add_subdirectories for libraries. Alternatively, use target_include_directories)
include_directories(${HEADER_PATH_WRAPPER})

# Miracle Core BLS381_32bits instantiation
SET(M_BLS381_32 "pfec_Miracl_Bls381_32")

SET(M_BLS381_32_SRC_PATH "${SRC_PATH_WRAPPER}/Miracl_BLS12381_32b")

add_library(${M_BLS381_32} STATIC 
        "${M_BLS381_32_SRC_PATH}/types.h"
        "${M_BLS381_32_SRC_PATH}/Zp.c"
        "${M_BLS381_32_SRC_PATH}/g1.c"
        "${M_BLS381_32_SRC_PATH}/g2.c"
        "${M_BLS381_32_SRC_PATH}/g3.c"
        "${M_BLS381_32_SRC_PATH}/pair.c"
        "${M_BLS381_32_SRC_PATH}/utils.c")

target_include_directories(${M_BLS381_32} PUBLIC ${HEADER_PATH_WRAPPER})

target_link_libraries(${M_BLS381_32}
        mcbls381_32b)

# Miracle Core BLS381_64bits instantiation
SET(M_BLS381_64 "pfec_Miracl_Bls381_64")

SET(M_BLS381_64_SRC_PATH "${SRC_PATH_WRAPPER}/Miracl_BLS12381_64b")

add_library(${M_BLS381_64} STATIC 
        "${M_BLS381_64_SRC_PATH}/types.h"
        "${M_BLS381_64_SRC_PATH}/Zp.c"
        "${M_BLS381_64_SRC_PATH}/g1.c"
        "${M_BLS381_64_SRC_PATH}/g2.c"
        "${M_BLS381_64_SRC_PATH}/g3.c"
        "${M_BLS381_64_SRC_PATH}/pair.c"
        "${M_BLS381_64_SRC_PATH}/utils.c")

target_include_directories(${M_BLS381_64} PUBLIC ${HEADER_PATH_WRAPPER})

target_link_libraries(${M_BLS381_64}
        mcbls381_64b)

# Example executable
add_executable(example_test 
        "${SRC_PATH_WRAPPER}/example/main_test.c")

target_link_libraries(example_test
        ${M_BLS381_64})

# Testing (with cmocka)
enable_testing()
add_subdirectory("${TEST_PATH_WRAPPER}/cmocka")

## Tests for pfec_Miracl_Bls381_32
add_cmocka_test(cmockaTestZp_BLS381_32 
                SOURCES "${TEST_PATH_WRAPPER}/testZp.c" 
                LINK_LIBRARIES ${M_BLS381_32} cmocka)
add_cmocka_test(cmockaTestG1_BLS381_32 
                SOURCES "${TEST_PATH_WRAPPER}/testG1.c" 
                LINK_LIBRARIES ${M_BLS381_32} cmocka) 
add_cmocka_test(cmockaTestG2_BLS381_32  
                SOURCES "${TEST_PATH_WRAPPER}/testG2.c" 
                LINK_LIBRARIES ${M_BLS381_32} cmocka)                 
add_cmocka_test(cmockaTestPair_BLS381_32  
                SOURCES "${TEST_PATH_WRAPPER}/testPair.c" 
                LINK_LIBRARIES ${M_BLS381_32} cmocka) 
## Tests for pfec_Miracl_Bls381_64
add_cmocka_test(cmockaTestZp_BLS381_64 
                SOURCES "${TEST_PATH_WRAPPER}/testZp.c" 
                LINK_LIBRARIES ${M_BLS381_64} cmocka)
add_cmocka_test(cmockaTestG1_BLS381_64 
                SOURCES "${TEST_PATH_WRAPPER}/testG1.c" 
                LINK_LIBRARIES ${M_BLS381_64} cmocka) 
add_cmocka_test(cmockaTestG2_BLS381_64  
                SOURCES "${TEST_PATH_WRAPPER}/testG2.c" 
                LINK_LIBRARIES ${M_BLS381_64} cmocka)                 
add_cmocka_test(cmockaTestPair_BLS381_64  
                SOURCES "${TEST_PATH_WRAPPER}/testPair.c" 
                LINK_LIBRARIES ${M_BLS381_64} cmocka) 



# Modifying output folders for libs/binaries
set_target_properties(${M_BLS381_32} example_test
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib/pfec_Miracl_Bls381_32"
LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib/pfec_Miracl_Bls381_32"
RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin/pfec_Miracl_Bls381_32"
)

set_target_properties(${M_BLS381_64} 
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib/pfec_Miracl_Bls381_64"
LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/lib/pfec_Miracl_Bls381_64"
RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output/bin/pfec_Miracl_Bls381_64"
)
