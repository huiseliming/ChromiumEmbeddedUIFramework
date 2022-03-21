cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# project ChromiumEmbeddedUIFramework
project(CeUIf)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(BinaryOutputDir ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${BinaryOutputDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BinaryOutputDir})

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

# disable sandbox
# add_compile_definitions(CEF_USE_SANDBOX=0)

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

ADD_LOGICAL_TARGET(libcef_lib ${CEF_LIB_DEBUG} ${CEF_LIB_RELEASE})
# Display configuration settings.
PRINT_CEF_CONFIG()

file(GLOB_RECURSE SRC src/*.h src/*.cpp)
list(FILTER SRC EXCLUDE REGEX "^.*src/SubProcess/.*")

add_library(CeUIf SHARED ${SRC})
add_dependencies(CeUIf libcef_dll_wrapper)
SET_EXECUTABLE_TARGET_PROPERTIES(CeUIf)

target_link_libraries(CeUIf libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

target_include_directories(CeUIf PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<INSTALL_INTERFACE:Include>    #${CMAKE_INSTALL_PREFIX}/include
)

# For windows manual export symbols
include(GenerateExportHeader)
generate_export_header(CeUIf 
    EXPORT_MACRO_NAME CEUIF_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Include/CeUIfExport.h)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ${CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS_SAVE})
endif()


# Add the custom manifest files to the executable.
#add_custom_command(
#    TARGET CeUIf
#    POST_BUILD
#    COMMAND "mt.exe" -nologo
#            -manifest \"${CMAKE_CURRENT_SOURCE_DIR}/CeUIf.exe.manifest\" \"${CMAKE_CURRENT_SOURCE_DIR}/compatibility.manifest\"
#            -outputresource:"${BinaryOutputDir}/CeUIf.exe"\;\#1
#    COMMENT "Adding manifest..."
#)

# Copy binary and resource files to the target output directory.
COPY_FILES("CeUIf" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${BinaryOutputDir}")
COPY_FILES("CeUIf" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${BinaryOutputDir}")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/html DESTINATION ${BinaryOutputDir}/Content)


# CeUIfSubProcess
file(GLOB_RECURSE CeUIfSubProcessSource src/SubProcess/*.h src/SubProcess/*.cpp)
add_executable(CeUIfSubProcess ${CeUIfSubProcessSource})
add_dependencies(CeUIfSubProcess libcef_dll_wrapper)
SET_EXECUTABLE_TARGET_PROPERTIES(CeUIfSubProcess)
target_link_libraries(CeUIfSubProcess CeUIf)