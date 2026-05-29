#pragma once
#include "ConstructionTool.h"
#include "geometry/LinearObject.h"

// Parallele zu einer Geraden durch einen Punkt.
// 1. Klick: Referenzgerade
// 2. Klick: Punkt durch den die Parallele läuft
class CreateParallelTool : public ConstructionTool {
    Q_OBJECT
    LinearObject* m_reference = nullptr;

protected:
    void onActivate() override;
    void onCancel() override;

    bool hasIntermediateState() const override { return m_reference != nullptr; }

public:
    explicit CreateParallelTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};