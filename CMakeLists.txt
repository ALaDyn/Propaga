cmake_minimum_required(VERSION 3.9)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(Propaga)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 1)
set(FIX_RELEASE 1)
set(RELEASE_DATE "2 May 2019")
set(RELEASE_NOTES "remove jsoncons from source tree")

# make sure that the default is a Release
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

configure_file (
  "${PROJECT_SOURCE_DIR}/src/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
  )

find_package(OpenMP)
find_package(MPI REQUIRED)
find_package(jsoncons REQUIRED)

file(GLOB Propaga_SOURCES "src/*.cpp")
file(GLOB Propaga_HEADERS "src/*.h")
add_executable(Propaga ${Propaga_SOURCES} ${Propaga_HEADERS})

target_include_directories(Propaga PRIVATE ${MPI_CXX_INCLUDE_PATH} ${PROJECT_BINARY_DIR})
target_compile_options(Propaga PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(Propaga PUBLIC ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
target_link_libraries(Propaga PRIVATE jsoncons)
if(OPENMP_FOUND)
  target_link_libraries(Propaga PUBLIC OpenMP::OpenMP_CXX)
else()
  target_compile_definitions(Propaga PRIVATE -DNO_OMP)
endif()
if(MSVC)
  target_compile_definitions(Propaga PRIVATE -D_SCL_SECURE_NO_WARNINGS)
  target_compile_definitions(Propaga PRIVATE -D_CRT_SECURE_NO_WARNINGS)
  target_compile_definitions(Propaga PRIVATE -D_USE_MATH_DEFINES)
endif()

install(TARGETS Propaga DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
