#pragma once

#include <limits>

namespace Constants::NumericConstants {
    constexpr double DOUBLE_EPS = std::numeric_limits<double>::epsilon();
    constexpr double DOUBLE_EPS_SQ = DOUBLE_EPS * DOUBLE_EPS;
}

namespace Constants::DrawingConstants {
    constexpr double FONT_SIZE = 8;
    constexpr double PENWIDTH_THIN = 0.5;
    constexpr double PENWIDTH_NORMAL = 1.5;
    constexpr double PENWIDTH_THICK = 2.5;
    constexpr double POINT_RADIUS = 4;
}

namespace Constants::ExportConstants {
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

namespace Constants::GeneralConstants {
    constexpr int RECENT_MAXCOUNT = 5;
}

namespace Constants::UiMetricsConstants {
    constexpr double SCENERECT_UNIT = 10000;
    constexpr double PASTE_OFFSET = 50;
    constexpr double HIT_TOLERANCE = 8;
    constexpr double SNAP_TOLERANCE = 15;
    constexpr double ZOOM_FACTOR = 1.15;
    constexpr double ZOOM_FACTOR_INV = 1.0 / ZOOM_FACTOR;
}

namespace Constants::LayoutConstants {
    constexpr int COLORBUTTON_WIDTH = 40;
    constexpr int COLORBUTTON_HEIGHT = 24;
    constexpr int SETTINGSDIALOG_MINWIDTH = 400;
}

namespace Constants::GeometryConstants {
    constexpr double LINE_EXTENT = 10000;
    constexpr double PARALLEL_EXTENT = 10000;
    constexpr double PERPENDICULAR_EXTENT = 10000;
}

namespace Constants::GridConstants {
    constexpr double EXTENT = 10000;
    constexpr double RASTER_START = 0.1;
    constexpr double SPACING = 50;
}


