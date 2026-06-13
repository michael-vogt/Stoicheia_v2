cmake_minimum_required(VERSION 3.24)
project(Stoicheia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------
# Qt
# -------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui LinguistTools Svg PrintSupport)

qt_standard_project_setup()
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# -------------------------------------------------
# Translation files
# -------------------------------------------------
set(TS_FILES
        translations/app_de.ts
        translations/app_en.ts
        translations/app_fr.ts
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
add_library(StoicheiaCore STATIC
        src/geometry/GeoObject.cpp
        src/geometry/Point.cpp
        src/geometry/LinearObject.cpp
        src/geometry/geometryHelper.cpp
        src/geometry/UpdateGuard.cpp
        src/geometry/Scene.cpp
        src/geometry/Circle.cpp

        src/constructions/Parallel.cpp
        src/constructions/Midpoint.cpp
        src/constructions/Perpendicular.cpp
        src/constructions/PerpendicularFoot.cpp
        src/constructions/IntersectionSet.cpp
        src/constructions/LineCircleIntersection.cpp
        src/constructions/LineLineIntersection.cpp
        src/constructions/CircleCircleIntersection.cpp
)

target_include_directories(StoicheiaCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(StoicheiaCore PUBLIC
        Qt6::Core
)

# -------------------------------------------------
# APP LIB (Commands, Tools, Serialization)
# -------------------------------------------------
add_library(StoicheiaApp STATIC
        src/ui/commands/CommandStack.cpp
        src/ui/commands/CreateCircleCommand.cpp
        src/ui/commands/CreateIntersectionCommand.cpp
        src/ui/commands/CreateLineCommand.cpp
        src/ui/commands/CreateMidpointCommand.cpp
        src/ui/commands/CreateParallelCommand.cpp
        src/ui/commands/CreatePerpendicularCommand.cpp
        src/ui/commands/CreatePerpendicularFootCommand.cpp
        src/ui/commands/CreatePointCommand.cpp
        src/ui/commands/DeleteObjectCommand.cpp
        src/ui/commands/MacroCommand.cpp
        src/ui/commands/MoveCenterCommand.cpp
        src/ui/commands/MovePointCommand.cpp

        src/ui/tools/ConstructionTool.cpp
        src/ui/tools/CreateCircleTool.cpp
        src/ui/tools/CreateIntersectionTool.cpp
        src/ui/tools/CreateLineTool.cpp
        src/ui/tools/CreateMidpointTool.cpp
        src/ui/tools/CreateParallelTool.cpp
        src/ui/tools/CreatePerpendicularFootTool.cpp
        src/ui/tools/CreatePerpendicularTool.cpp
        src/ui/tools/CreatePointTool.cpp
        src/ui/tools/SelectTool.cpp

        src/ui/io/ExportManager.cpp
        src/ui/io/FileManager.cpp
        src/ui/io/FileManager.h
        src/ui/io/PngExporter.cpp
        src/ui/io/PdfExporter.cpp
        src/ui/io/Serializer.cpp
        src/ui/io/SvgExporter.cpp

        src/ui/SnapHelper.cpp
        src/ui/Grid.cpp
        src/ui/commands/CopyCommand.cpp
        src/ui/commands/MergePointsCommand.cpp
        src/ui/InputManager.cpp
)

target_include_directories(StoicheiaApp PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(StoicheiaApp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/dialogs
)

target_link_libraries(StoicheiaApp PRIVATE
        StoicheiaCore
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
        Qt6::PrintSupport
)


# -------------------------------------------------
# GUI LIB (Qt Widgets / Items)
# -------------------------------------------------
add_library(StoicheiaQt STATIC
src/ui/dialogs/AppSettings.cpp
        src/ui/dialogs/AppSettings.h
        src/ui/dialogs/ColorButton.cpp
        src/ui/dialogs/ColorButton.h
        src/ui/dialogs/CoordinateInputDialog.cpp
        src/ui/dialogs/CoordinateInputDialog.h
        src/ui/dialogs/SettingsDialog.cpp
        src/ui/dialogs/SettingsDialog.h
        src/ui/dialogs/SettingsDialog.ui
        src/ui/GeoGraphicsItem.cpp
        src/ui/GeoPointItem.cpp
        src/ui/GeoLinearObjectItem.cpp
        src/ui/GeoCircleItem.cpp
        src/ui/GeoIntersectionPointItem.cpp
        src/ui/SceneAdapter.cpp
        src/ui/HitTest.cpp
        src/ui/DrawingBoard.cpp
        src/ui/DrawingBoard.h
        src/ui/MainWindow.cpp
        src/ui/MainWindow.h
        src/ui/MainWindow.ui
)

target_include_directories(StoicheiaQt PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(StoicheiaQt PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/dialogs
)

target_link_libraries(StoicheiaQt PUBLIC
        Qt6::Core
        Qt6::Widgets
        StoicheiaCore
        StoicheiaApp
)

# -------------------------------------------------
# EXECUTABLE (Application)
# -------------------------------------------------
qt_add_executable(Stoicheia
        main.cpp
        resources.qrc
)

target_link_libraries(Stoicheia PRIVATE
        StoicheiaCore
        StoicheiaApp
        StoicheiaQt
        Qt6::Core
        Qt6::Widgets
)

target_sources(Stoicheia PRIVATE resources/app.rc)

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
    target_link_libraries(${target} PRIVATE StoicheiaCore gtest_main)

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
target_link_libraries(AllTests PRIVATE StoicheiaCore gtest_main)

gtest_discover_tests(AllTests)