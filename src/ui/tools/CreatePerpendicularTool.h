#pragma once
#include "ConstructionTool.h"
#include "geometry/LinearObject.h"

// Senkrechte zu einer Geraden durch einen Punkt.
// 1. Klick: Referenzgerade
// 2. Klick: Punkt durch den die Senkrechte läuft
class CreatePerpendicularTool : public ConstructionTool {
    LinearObject* m_reference = nullptr;

protected:
    void onActivate() override;
    void onCancel() override;

public:
    explicit CreatePerpendicularTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};