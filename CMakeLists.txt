cmake_minimum_required(VERSION 4.2)
project(Stoicheia)

set(CMAKE_CXX_STANDARD 26)

# ---------------------------
# Hauptcode als Library
# ---------------------------
add_library(
        StoicheiaLib
        src/geometry/GeoObject.cpp
        src/geometry/Point.cpp
        src/geometry/Line.cpp
        src/geometry/geometryHelper.cpp
        src/geometry/UpdateGuard.cpp
        src/geometry/Scene.cpp
        src/constructions/IntersectionPoint.cpp
        src/constructions/Parallel.cpp
        src/constructions/Midpoint.cpp
        src/constructions/Perpendicular.cpp
        src/constructions/PerpendicularFoot.cpp
        src/geometry/Circle.cpp
        src/constructions/IntersectionSet.cpp
        src/constructions/LineCircleIntersection.cpp
        src/constructions/LineLineIntersection.cpp
        src/constructions/CircleCircleIntersection.cpp
)

target_include_directories(StoicheiaLib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------------------
# Hauptprogramm
# ---------------------------
add_executable(Stoicheia main.cpp)

target_link_libraries(Stoicheia PRIVATE StoicheiaLib)

# ---------------------------
# GoogleTest
# ---------------------------
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ---------------------------
# Tests
# ---------------------------
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(StoicheiaTests ${TEST_SOURCES})

target_link_libraries(StoicheiaTests
        PRIVATE
        StoicheiaLib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(StoicheiaTests)