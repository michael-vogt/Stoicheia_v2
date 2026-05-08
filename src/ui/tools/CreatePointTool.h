#pragma once
#include "Tool.h"

class CreatePointTool : public Tool {
public:
    explicit CreatePointTool(const ToolContext& ctx);

    void activate() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QCursor cursor() const override;
};
