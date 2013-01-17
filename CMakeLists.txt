########################################
# set up the project basic information #
########################################

# project name
PROJECT(MAXSUM-CPP)

# minimum cmake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# add or remove debugging info
SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE Release)

# verbose makefile
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# output directory for binaries and libraries
SET(BIN ${CMAKE_SOURCE_DIR}/bin)
SET(LIB ${CMAKE_SOURCE_DIR}/lib)

# find eigen linear algebra library
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 3.1.0)

###########################################
# Generate Documentation                  #
###########################################
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
   ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMENT "Generating API documentation with Doxygen" VERBATIM
   )
endif(DOXYGEN_FOUND)

###########################################
# Dump exec and libraries in the same place
###########################################

# set the output directories
SET(LIBRARY_OUTPUT_PATH ${LIB} CACHE PATH "Output directory for the libraries")
SET(EXECUTABLE_OUTPUT_PATH ${BIN} CACHE PATH "Output directory for the executables")

# allow linking to these directories
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${EIGEN3_INCLUDE_DIR})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

############################
# make an uninstall target #
############################

#CONFIGURE_FILE(
# 	"${CMAKE_CURRENT_SOURCE_DIR}/UninstallScript.cmake.in"
#  	"${CMAKE_CURRENT_BINARY_DIR}/UninstallScript.cmake"
#  	IMMEDIATE @ONLY
#)

#ADD_CUSTOM_TARGET(uninstall
#  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/UninstallScript.cmake")

#############################
# compile project library   #
#############################
FILE(GLOB MAX_SUM_SRC src/*.cpp)
ADD_LIBRARY(MaxSum SHARED ${MAX_SUM_SRC})

###############################
# build test harnesses        #
###############################
ADD_EXECUTABLE(utilHarness tests/utilHarness.cpp)
ADD_EXECUTABLE(stdHarness tests/stdHarness.cpp)
ADD_EXECUTABLE(funHarness tests/funHarness.cpp)
ADD_EXECUTABLE(mathHarness tests/mathHarness.cpp)
ADD_EXECUTABLE(domainHarness tests/domainHarness.cpp)
ADD_EXECUTABLE(agg1Harness tests/agg1Harness.cpp)
ADD_EXECUTABLE(agg2Harness tests/agg2Harness.cpp)
ADD_EXECUTABLE(postHarness tests/postHarness.cpp)
ADD_EXECUTABLE(maxsumHarness tests/maxsumHarness.cpp)
ADD_EXECUTABLE(limitsHarness tests/limitsHarness.cpp)
TARGET_LINK_LIBRARIES (utilHarness MaxSum)
TARGET_LINK_LIBRARIES (funHarness MaxSum)
TARGET_LINK_LIBRARIES (stdHarness MaxSum)
TARGET_LINK_LIBRARIES (mathHarness MaxSum)
TARGET_LINK_LIBRARIES (domainHarness MaxSum)
TARGET_LINK_LIBRARIES (agg1Harness MaxSum)
TARGET_LINK_LIBRARIES (agg2Harness MaxSum)
TARGET_LINK_LIBRARIES (postHarness MaxSum)
TARGET_LINK_LIBRARIES (maxsumHarness MaxSum)
TARGET_LINK_LIBRARIES (limitsHarness MaxSum)

###############################
# enable testing              #
###############################
ENABLE_TESTING()
ADD_TEST(LIMITS_TEST ${CMAKE_SOURCE_DIR}/bin/limitsHarness)
ADD_TEST(STDLIB_TEST ${CMAKE_SOURCE_DIR}/bin/stdHarness)
ADD_TEST(MATH_TEST ${CMAKE_SOURCE_DIR}/bin/mathHarness)
ADD_TEST(UTIL_TEST ${CMAKE_SOURCE_DIR}/bin/utilHarness)
ADD_TEST(FUNCTION_TEST ${CMAKE_SOURCE_DIR}/bin/funHarness)
ADD_TEST(DOMAIN_TEST ${CMAKE_SOURCE_DIR}/bin/domainHarness)
ADD_TEST(AGG1_TEST ${CMAKE_SOURCE_DIR}/bin/agg1Harness)
ADD_TEST(AGG2_TEST ${CMAKE_SOURCE_DIR}/bin/agg2Harness)
ADD_TEST(POST_TEST ${CMAKE_SOURCE_DIR}/bin/postHarness)
ADD_TEST(MAXSUM_TEST ${CMAKE_SOURCE_DIR}/bin/maxsumHarness)

