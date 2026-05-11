cmake_minimum_required(VERSION 3.24)
project(Stoicheia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------
# Qt
# -------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

# -------------------------------------------------
# Translation files
# -------------------------------------------------
set(TS_FILES
        translations/app_de.ts
        translations/app_en.ts
)

# -------------------------------------------------
# Manual i18n workflow (Qt6 stable + CLion friendly)
# -------------------------------------------------
add_custom_target(update_translations
        COMMAND Qt6::lupdate
        ${CMAKE_SOURCE_DIR}/src
        -ts ${TS_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(release_translations
        COMMAND Qt6::lrelease
        ${TS_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# -------------------------------------------------
# CORE LIB (Geometry / Logic)
# -------------------------------------------------
add_library(StoicheiaLib STATIC
        src/geometry/GeoObject.cpp
        src/geometry/Point.cpp
        src/geometry/LinearObject.cpp
        src/geometry/geometryHelper.cpp
        src/geometry/UpdateGuard.cpp
        src/geometry/Scene.cpp

        src/constructions/Parallel.cpp
        src/constructions/Midpoint.cpp
        src/constructions/Perpendicular.cpp
        src/constructions/PerpendicularFoot.cpp

        src/geometry/Circle.cpp
        src/constructions/IntersectionSet.cpp
        src/constructions/LineCircleIntersection.cpp
        src/constructions/LineLineIntersection.cpp
        src/constructions/CircleCircleIntersection.cpp

        src/geometry/Triangle.cpp
        src/geometry/Polygon.cpp
)

target_include_directories(StoicheiaLib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(StoicheiaLib PUBLIC
        Qt6::Core
)

# -------------------------------------------------
# GUI LIB (Qt Widgets / Items)
# -------------------------------------------------
add_library(StoicheiaGui STATIC
        src/ui/GeoGraphicsItem.cpp
        src/ui/GeoPointItem.cpp
        src/ui/GeoLinearObjectItem.cpp
        src/ui/GeoCircleItem.cpp
        src/ui/GeoIntersectionPointItem.cpp
        src/ui/SceneAdapter.cpp
)

target_include_directories(StoicheiaGui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(StoicheiaGui PUBLIC
        Qt6::Core
        Qt6::Widgets
        StoicheiaLib
)

# -------------------------------------------------
# EXECUTABLE (Application)
# -------------------------------------------------
qt_add_executable(Stoicheia WIN32
        main.cpp

        src/ui/MainWindow.cpp
        src/ui/MainWindow.h
        src/ui/DrawingBoard.cpp
        src/ui/DrawingBoard.h

        src/ui/commands/CommandStack.cpp
        src/ui/commands/CreatePointCommand.cpp
        src/ui/commands/MovePointCommand.cpp
        src/ui/commands/DeleteObjectCommand.cpp
        src/ui/commands/CreateLineCommand.cpp
        src/ui/commands/CreateCircleCommand.cpp
        src/ui/commands/MoveCenterCommand.cpp
        src/ui/commands/MacroCommand.cpp

        src/ui/tools/SelectTool.cpp
        src/ui/tools/CreatePointTool.cpp
        src/ui/tools/CreateLineTool.cpp
        src/ui/tools/CreateCircleTool.cpp

        src/ui/SnapHelper.cpp
)

target_link_libraries(Stoicheia PRIVATE
        StoicheiaLib
        StoicheiaGui
        Qt6::Core
        Qt6::Widgets
)

# -------------------------------------------------
# Qt Translations (RICHTIG + MODERN)
# -------------------------------------------------
qt_add_translations(Stoicheia
        TS_FILES ${TS_FILES}
        RESOURCE_PREFIX "/i18n"
)

# -------------------------------------------------
# Windows deployment
# -------------------------------------------------
if (WIN32)
    add_custom_command(TARGET Stoicheia POST_BUILD
            COMMAND ${Qt6_DIR}/../../../bin/windeployqt.exe
            $<TARGET_FILE:Stoicheia>
            COMMENT "Running windeployqt..."
    )
endif()

# -------------------------------------------------
# GoogleTest
# -------------------------------------------------
include(FetchContent)
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

function(add_geo_test target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE StoicheiaLib gtest_main)

    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    gtest_discover_tests(${target})
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