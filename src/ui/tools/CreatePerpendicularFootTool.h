#pragma once
#include "ConstructionTool.h"

// Lotfußpunkt: orthogonale Projektion eines Punktes auf eine Gerade.
// 1. Klick: Punkt
// 2. Klick: Gerade
class CreatePerpendicularFootTool : public ConstructionTool {
    Q_OBJECT
    Point* m_point = nullptr;

protected:
    void onActivate() override;
    void onCancel() override;

    bool hasIntermediateState() const override { return m_point != nullptr; }

public:
    explicit CreatePerpendicularFootTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};