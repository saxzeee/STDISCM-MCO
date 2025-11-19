# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

#Add static runtimes
#Runtime DLLs on build
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

#Project name
project ("STDISCM-SFML-CMAKE")

#Change the output directory of the exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Set base path for 3rd party reference projects
include(FetchContent)
#Set directory of third party stuff
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Base Directory of libs" FORCE)
#Set directory of assets
set(RESOURCES_BASE_DIR ${CMAKE_CURRENT_DIR}/Media CACHE PATH "Base Directory of resources" FORCE)

#Set the source path for the project
set(PROJ_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src CACHE PATH "Project SRC" FORCE)

#Add everything in the src folder
file(GLOB_RECURSE PROJ_SRC CONFIGURE_DEPENDS "${PROJ_SRC_PATH}/*.[ch]pp")

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable (STDISCM-SFML-CMAKE "${PROJ_SRC}" "STDISCM-SFML-CMAKE.cpp" "STDISCM-SFML-CMAKE.h")

#add_executable (STDISCM-SFML-CMAKE "${PROJ_SRC}" "Test.cpp" "STDISCM-SFML-CMAKE.h")

#Change Trarget C++ compile to ver 20
target_compile_features(STDISCM-SFML-CMAKE PRIVATE cxx_std_20)
#Link the SFML graphics lib and audio
# target_link_libraries(STDISCM-SFML-CMAKE PRIVATE sfml-graphics)
target_link_libraries(STDISCM-SFML-CMAKE PRIVATE sfml-graphics sfml-audio)

add_custom_command(
    TARGET STDISCM-SFML-CMAKE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Media
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Media
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET STDISCM-SFML-CMAKE PROPERTY CXX_STANDARD 20)
endif()

#Include sub-projects.
#add_subdirectory ("CMakeProject1")

# Add source to this project's executable.
#add_executable (CMakeProject1 "CMakeProject1.cpp" "CMakeProject1.h")

# TODO: Add tests and install targets if needed.

