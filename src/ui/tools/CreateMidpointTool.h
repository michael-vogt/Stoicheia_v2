#pragma once
#include "ConstructionTool.h"


class CreateMidpointTool : public ConstructionTool {

    Q_OBJECT

public:
    explicit CreateMidpointTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

protected:
    void onActivate() override { showStatus(tr("Ersten Punkt klicken")); }
    void onCancel() override { m_firstPoint = nullptr; }
    [[nodiscard]] auto hasIntermediateState() const -> bool override { return m_firstPoint != nullptr; }

private:
    Point* m_firstPoint = nullptr;
};