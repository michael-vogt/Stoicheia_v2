#pragma once
#include "ConstructionTool.h"

// Schnittpunkt zweier Objekte erzeugen.
// 1. Klick: erstes Objekt (Line oder Circle)
// 2. Klick: zweites Objekt (Line oder Circle)
class CreateIntersectionTool : public ConstructionTool {
    GeoObject* m_firstObject = nullptr;

protected:
    void onActivate() override;
    void onCancel() override;

public:
    explicit CreateIntersectionTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};