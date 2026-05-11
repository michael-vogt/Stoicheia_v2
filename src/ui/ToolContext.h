#pragma once
#include "SnapHelper.h"

class DrawingBoard;
class SceneAdapter;
class CommandStack;

struct ToolContext {
    DrawingBoard* drawingBoard;
    SceneAdapter* adapter;
    CommandStack* commandStack;
    SnapHelper* snapHelper;
};