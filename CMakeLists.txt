# Minimum required version of CMake
cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Project name and version
project(corral VERSION 1.0)

set(EXECUTABLE_NAME ${PROJECT_NAME})
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "") 
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

# Set C++ standard (e.g., C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Include the include/ directory for header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Gather all source files in the src/ directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

if (EMSCRIPTEN)
    # Add the lib/web directory to the library search path
    link_directories(${CMAKE_SOURCE_DIR}/lib/web)
else()
    # Add the lib/ directory to the library search path
    link_directories(${CMAKE_SOURCE_DIR}/lib)
endif()

# Create an executable from the source files
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory("deps/SDL" EXCLUDE_FROM_ALL)
set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
add_subdirectory("deps/SDL_ttf" EXCLUDE_FROM_ALL)	
add_subdirectory("deps/yoga" EXCLUDE_FROM_ALL)	
if (NOT TARGET plutovg)
   add_subdirectory("deps/plutovg" EXCLUDE_FROM_ALL)	
endif()

# Handle resources for WebAssembly
if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 --preload-file res --shell-file ${CMAKE_SOURCE_DIR}/minshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
endif()

# Link libraries to the executable
target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
# Link SDL to our executable. This also makes its include directory available to us. 
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf 
    plutovg 
    yogacore
)

if (APPLE)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework Cocoa"
        "-weak_framework UniformTypeIdentifiers"
        "-framework IOKit"
        "-framework ForceFeedback"
        "-framework Carbon"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework Foundation"
        "-framework GameController"
        "-framework Metal"
        "-framework QuartzCore"
        "-weak_framework CoreHaptics"
        pthread
        m
    )
endif()

# Copy the resources directory to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

# Optional: Add a target for running the executable
add_custom_target(run
    COMMAND ${EXECUTABLE_NAME}
    DEPENDS ${EXECUTABLE_NAME}
)
