cmake_minimum_required(VERSION 3.12)

# Set project name
project(rjmcmcmt)

set(LIBRJMCMC_ROOT $ENV{LIBRJMCMC_ROOT})
message(STATUS LIBRJMCMC_ROOT = ${LIBRJMCMC_ROOT})

message(STATUS Build Type = ${CMAKE_BUILD_TYPE})

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Configure find package
find_package(PkgConfig REQUIRED)

# Set options
option(USE_MPI "Use MPI" ON)
message(STATUS USE_MPI = ${USE_MPI})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-compare -Wno-format-security")
SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -O3 -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-compare -Wno-format-security")
if(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result -Wno-date-time")
	SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-unused-result -Wno-date-time")
	#On GCC, even with -Wno-date-time, still get warings of the form: warning: macro "__DATE__" might prevent reproducible builds [-Wdate-time]
endif()

# Adding filesystem library
link_libraries(-lstdc++fs)

# Configure MPI if opted for
find_package(MPI)
if(MPI_FOUND)
	message(STATUS "MPI was found")
	include_directories(SYSTEM ${MPI_INCLUDE_PATH})
	add_definitions(-D_MPI_ENABLED -DOMPI_SKIP_MPICXX)
	link_libraries(${MPI_C_LIBRARIES})
endif()


# Add the include directories
include_directories(src/)
include_directories(submodules/cpp-utils/src/)


# Add the cpp-utils library submodule
add_subdirectory(submodules/cpp-utils EXCLUDE_FROM_ALL)

# Add the executables
set(target "rjmcmcmt")
add_executable(${target}.exe src/${target}.cpp)
target_include_directories(${target}.exe PRIVATE ${LIBRJMCMC_ROOT}/include)
target_link_libraries(${target}.exe PRIVATE cpp-utils-static)
target_link_libraries(${target}.exe PRIVATE ${LIBRJMCMC_ROOT}/lib/librjmcmc.a)
install(TARGETS ${target}.exe DESTINATION bin)

set(target "generate_synthetic_data")
add_executable(${target}.exe src/${target}.cpp)
target_link_libraries(${target}.exe PRIVATE cpp-utils-static)
install(TARGETS ${target}.exe DESTINATION bin)

install(DIRECTORY examples DESTINATION .)
install(DIRECTORY matlab DESTINATION .)
install(DIRECTORY python DESTINATION .)

