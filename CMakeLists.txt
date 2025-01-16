# # Set the minimum version of CMake required
# cmake_minimum_required(VERSION 3.10)

# # Define the project name and version
# project(DeribitOrderSystem VERSION 1.0)

# # Set the C++ standard to C++11 (or higher)
# set(CMAKE_CXX_STANDARD 17)

# # Add the source files
# set(SOURCES
#     src/main.cpp
#     src/api_handler.cpp
#     src/websocket_server.cpp
#     src/order_manager.cpp
#     src/logger.cpp
#     src/config.cpp
# )

# # Create the executable
# add_executable(DeribitOrderSystem ${SOURCES})

# # Include directories
# include_directories(include)

# # Specify paths to libcurl and libwebsockets headers and libraries
# # Adjust these paths to where you have the libcurl and libwebsockets libraries
# set(CURL_INCLUDE_DIR "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/include")
# set(LWS_INCLUDE_DIR "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/include")
# set(CURL_LIB_DIR "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/lib")
# set(LWS_LIB_DIR "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/lib")

# # Include the directories for header files
# include_directories(${CURL_INCLUDE_DIR} ${LWS_INCLUDE_DIR})

# # Link directories for .lib files
# link_directories(${CURL_LIB_DIR} ${LWS_LIB_DIR})

# # Add the required libraries
# # Make sure you link libcurl and libwebsockets properly (you might need to use static or dynamic libraries)
# target_link_libraries(DeribitOrderSystem
#     ws2_32.lib         # Windows networking library (required for socket communication)
#     libcurl.lib        # Link to libcurl (adjust the name if necessary)
#     websockets.lib  # Link to libwebsockets (adjust the name if necessary)
# )

# # Set output directory for the executable
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# # Optional: Add a post-build step to copy DLLs if using dynamic linking
# # For example, if you're using libcurl.dll and libwebsockets.dll, you can copy them
# # to the bin directory after building the project
# add_custom_command(TARGET DeribitOrderSystem POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/bin/libcurl.dll"   # Path to libcurl.dll
#     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
# )
# add_custom_command(TARGET DeribitOrderSystem POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/bin/websockets.dll"   # Path to libwebsockets.dll
#     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
# )

# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name and version
project(DeribitOrderSystem VERSION 1.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
set(SOURCES
    src/main.cpp
    src/api_handler.cpp
    src/websocket_server.cpp
    src/order_manager.cpp
    src/logger.cpp
    src/config.cpp
)

# Add header files
set(HEADERS
    include/api_handler.h
    include/websocket_server.h
    include/order_manager.h
    include/logger.h
    include/config.h
)

# Create the executable
add_executable(DeribitOrderSystem ${SOURCES} ${HEADERS})

# Use vcpkg toolchain file if available
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Find required packages
find_package(CURL REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(libwebsockets CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Boost REQUIRED COMPONENTS beast)
find_package(Boost REQUIRED COMPONENTS json move)
find_package(nlohmann_json CONFIG REQUIRED)


# Link required libraries
target_link_libraries(DeribitOrderSystem PRIVATE CURL::libcurl websockets_shared JsonCpp::JsonCpp cpprestsdk::cpprest Boost::move Boost::json Boost::system Boost::thread nlohmann_json::nlohmann_json)

# Include directories for headers
target_include_directories(DeribitOrderSystem PRIVATE include)

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Optional: Copy dependent DLLs to the output directory (for dynamic linking)
add_custom_command(TARGET DeribitOrderSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/bin/libcurl.dll"   # Hardcoded path to libcurl.dll
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
add_custom_command(TARGET DeribitOrderSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/bin/websockets.dll"   # Hardcoded path to websockets.dll
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
add_custom_command(TARGET DeribitOrderSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/bin/jsoncpp.dll"
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
add_custom_command(TARGET DeribitOrderSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Users/bansaldi/Desktop/DerbinTest/vcpkg/installed/x64-windows/bin/cpprest_2_10.dll"
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

