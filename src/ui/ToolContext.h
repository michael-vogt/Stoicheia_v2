#pragma once
#include "SnapHelper.h"

class DrawingBoard;
class SceneAdapter;
class CommandStack;
class HitTest;

struct ToolContext {
    DrawingBoard* drawingBoard;
    SceneAdapter* adapter;
    CommandStack* commandStack;
    SnapHelper* snapHelper;
    HitTest* hitTest;
};