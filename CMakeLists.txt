cmake_minimum_required(VERSION 3.26)

project(HinaPE-jet)

set(CMAKE_CXX_STANDARD 17)
set(TARGET SIM_PBFSolver)

# Find Houdini
if (MSVC)
    set(Houdini_PATH "C:/Program Files/Side Effects Software/Houdini 20.0.547")
elseif (APPLE)
    set(Houdini_PATH "/Applications/Houdini/Houdini20.0.547/Frameworks/Houdini.framework/Versions/20.0/Resources")
endif ()
set(Houdini_DIR ${Houdini_PATH}/toolkit/cmake)
find_package(Houdini REQUIRED)

# Load Source
add_library(
        ${TARGET}
        SHARED
        Entrance.cpp
        SIM_JETSPHSolver.cpp
        SIM_JETSPHSolver.h
        hdk_interface/universal_array_data.h
)

# Link Houdini Toolkit
target_link_libraries(
        ${TARGET}
        PUBLIC
        Houdini
        jet
)
target_link_directories(
        ${TARGET}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        install/lib
)
target_include_directories(
        ${TARGET}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        install/include
)
target_compile_options(
        ${TARGET}
        PRIVATE
        -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
)
houdini_configure_target(${TARGET})
