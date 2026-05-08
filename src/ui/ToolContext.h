#pragma once

class DrawingBoard;
class SceneAdapter;
class CommandStack;

struct ToolContext {
    DrawingBoard* drawingBoard;
    SceneAdapter* adapter;
    CommandStack* commandStack;
};