#pragma once
#include "ConstructionTool.h"


class CreatePerpendicularFootTool : public ConstructionTool {
    Q_OBJECT

public:
    explicit CreatePerpendicularFootTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override { event->ignore(); }

protected:
    void onActivate() override { showStatus(tr("Punkt klicken")); }
    void onCancel() override { m_point = nullptr; }
    bool hasIntermediateState() const override { return m_point != nullptr; }

private:
    Point* m_point = nullptr;
};