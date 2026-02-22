cmake_minimum_required(VERSION 3.10)
project(SecureFileMatcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(SecureFileMatcher
    src/main.cpp
    src/Matcher.cpp
    src/HttpServer.cpp
)

# Link Winsock library for Windows
if(WIN32)
    target_link_libraries(SecureFileMatcher ws2_32)
endif()
