#pragma once

#include <qcolor.h>

namespace Constants::DrawingConstants {
    constexpr double FONT_SIZE = 8;
    constexpr double PENWIDTH_THIN = 0.5;
    constexpr double PENWIDTH_NORMAL = 1.5;
    constexpr double PENWIDTH_THICK = 2.5;
    constexpr double POINT_RADIUS = 4;
}

namespace Constants::Export {
    namespace PNG {
        constexpr double DPI = 96;
        constexpr double MARGIN = 20;
    }

    namespace SVG {
        constexpr double BOUNDINGBOX_WIDTH = 1e10;
        constexpr double MINSIZE = 100;
        constexpr double MARGIN = 50;
        constexpr double LINE_EXTENT = 10000;
    }
}

namespace Constants::General {
    constexpr int RECENT_MAXCOUNT = 5;
}

namespace Constants::UiMetrics {
    constexpr double SCENERECT_UNIT = 10000;
    constexpr double PASTE_OFFSET = 50;
    constexpr double HIT_TOLERANCE = 8;
    constexpr double SNAP_TOLERANCE = 15;
    constexpr double ZOOM_FACTOR = 1.5;
    constexpr double ZOOM_FACTOR_INV = 1.0 / 1.5;
}

namespace Constants::LayoutConstants {
    constexpr int COLORBUTTON_WIDTH = 40;
    constexpr int COLORBUTTON_HEIGHT = 24;
    constexpr int SETTINGSDIALOG_MINWIDTH = 400;
}

namespace Constants::Geometry {
    constexpr double LINE_EXTENT = 10000;
    constexpr double PARALLEL_EXTENT = 10000;
    constexpr double PERPENDICULAR_EXTENT = 10000;
}

namespace Constants::ColorScheme {
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

namespace Constants::Grid {
    constexpr double EXTENT = 10000;
    constexpr double RASTER_START = 0.1;
    constexpr double SPACING = 50;
}


