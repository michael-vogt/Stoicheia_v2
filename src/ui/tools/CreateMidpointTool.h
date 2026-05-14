#pragma once
#include "ConstructionTool.h"

// Mittelpunkt zweier Punkte erzeugen.
// 1. Klick: erster Punkt
// 2. Klick: zweiter Punkt → Mittelpunkt wird erzeugt
class CreateMidpointTool : public ConstructionTool {
    Point* m_firstPoint = nullptr;

protected:
    void onActivate() override;
    void onCancel() override;

    bool hasIntermediateState() const override { return m_firstPoint != nullptr; }

public:
    explicit CreateMidpointTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};