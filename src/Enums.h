#pragma once

#include <cstdint>

enum class ShortcutMode : std::uint8_t {
    None,
    Geometry,
    Construction
};

enum class StatusBarPart : std::uint8_t {
    Left,
    Right
};

enum class ToolType : std::uint8_t {
    Select,
    CreatePoint,
    CreateLine,
    CreateRay,
    CreateSegment,
    CreateCircle,
    CreateIntersection,
    CreateMidpoint,
    CreateParallel,
    CreatePerpendicular,
    CreatePerpendicularFoot
};