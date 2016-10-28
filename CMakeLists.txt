cmake_minimum_required(VERSION 3.2)
project(mimeutil)

option(MIMEUTIL_BUILD_TESTS "Build tests" OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_library(mimeutil STATIC
    mimeutil/MimeParser.cpp
    mimeutil/MimeParser.h
    mimeutil/MimeWriter.cpp
    mimeutil/MimeWriter.h
    mimeutil/MimeInfo.cpp
    mimeutil/MimeInfo.h
)

install(DIRECTORY mimeutil DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS mimeutil DESTINATION lib)

if (MIMEUTIL_BUILD_TESTS)
    add_executable(mimeparser tests/mimeparser.cpp)
    target_link_libraries(mimeparser mimeutil)
endif()
