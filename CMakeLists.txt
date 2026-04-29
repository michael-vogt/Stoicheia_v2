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
        src/geometry/LinearObject.cpp
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
        src/geometry/Line.cpp
        src/geometry/Ray.cpp
        src/geometry/Segment.cpp
        src/geometry/Triangle.cpp
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
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

# ---------------------------
# Tests
# ---------------------------
include(GoogleTest)
enable_testing()

function (add_geo_test target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE StoicheiaLib gtest_main)
    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    gtest_discover_tests(${target} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    #gtest_add_tests(TARGET ${target} SOURCES ${source})
endfunction()

add_geo_test(CircleTest tests/circletests.cpp)
add_geo_test(ConstructionTest tests/constructiontests.cpp)
add_geo_test(ExtendedTest tests/extendedtests.cpp)
add_geo_test(GeometryTest tests/geometrytests.cpp)
add_geo_test(LineTest tests/linetest.cpp)
add_geo_test(LinearTest tests/lineartest.cpp)

file(GLOB ALL_TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(AllTests ${ALL_TEST_SOURCES})
target_link_libraries(AllTests PRIVATE StoicheiaLib gtest_main)
gtest_discover_tests(AllTests)
#add_executable(StoicheiaTests ${TEST_SOURCES})

#target_link_libraries(StoicheiaTests
#        PRIVATE
#        StoicheiaLib
#        gtest_main
#)

#include(GoogleTest)
#gtest_discover_tests(StoicheiaTests)