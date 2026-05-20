#pragma once

enum class ShortcutMode {
    None,
    Geometry,
    Construction
};

enum class StatusBarPart {
    Left,
    Right
};

enum class ToolType {
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