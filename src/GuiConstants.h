#pragma once

#include <QColor>

namespace GuiConstants::Colors {
    constexpr QColor BACKGROUND(245, 245, 245);
    constexpr QColor POINT(0, 0, 0);
    constexpr QColor LINE(0, 0, 0);
    constexpr QColor CIRCLE(0, 0, 0);
    constexpr QColor POINTFILL(255, 255, 255);
    constexpr QColor SELECT(0,0,255);
    constexpr QColor HIGHLIGHT(255, 140, 0);
    constexpr QColor CONSTRUCTION(100, 100, 200);
    constexpr QColor WATRMARK(0, 0, 0, 20);

    constexpr QColor AXISCOLOR(160, 160, 160);
    constexpr QColor GRIDCOLOR(220, 220, 220);
    constexpr QColor LABELCOLOR(120, 120, 120);

    constexpr double WATERMARK_OPACITY = 0.1;
}