cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ChromiumEmbeddedUIFramework)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)



#
# CEF configuration.
#

# Specify the CEF distribution version.
set(CEF_VERSION "99.2.11+g35f9195+chromium-99.0.4844.51")

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  if("${PROJECT_ARCH}" STREQUAL "arm64")
    set(CEF_PLATFORM "macosarm64")
  else()
    set(CEF_PLATFORM "macosx64")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "linux64")
  else()
    set(CEF_PLATFORM "linux32")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "windows64")
  else()
    set(CEF_PLATFORM "windows32")
  endif()
endif()

# Add this project's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Download and extract the CEF binary distribution (executes DownloadCEF.cmake).
include(DownloadCEF)
DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_SOURCE_DIR}/third_party/cef")

# Add the CEF binary distribution's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Load the CEF configuration (executes FindCEF.cmake).
find_package(CEF REQUIRED)

#
# Target configuration.
#

# Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt).
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Include CEF's test application targets (executes <target>/CMakeLists.txt).
add_subdirectory(${CEF_ROOT}/tests/cefsimple)

# Allow includes relative to the current source directory.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Display configuration settings.
PRINT_CEF_CONFIG()

file(GLOB_RECURSE SRC src/*.h src/*.cpp)

add_executable(ChromiumEmbeddedUIFramework ${SRC})
add_dependencies(ChromiumEmbeddedUIFramework libcef_dll_wrapper)
SET_EXECUTABLE_TARGET_PROPERTIES(ChromiumEmbeddedUIFramework)
target_link_libraries(ChromiumEmbeddedUIFramework libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
# Add the custom manifest files to the executable.
ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "ChromiumEmbeddedUIFramework" "exe")
# Copy binary and resource files to the target output directory.
COPY_FILES("ChromiumEmbeddedUIFramework" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
COPY_FILES("ChromiumEmbeddedUIFramework" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
