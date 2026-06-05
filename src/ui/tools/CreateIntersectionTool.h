#pragma once
#include "ConstructionTool.h"

class CreateIntersectionTool : public ConstructionTool {

    Q_OBJECT

public:
    explicit CreateIntersectionTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override { event->ignore(); }

protected:
    void onActivate() override { showStatus(tr("Erstes Objekt klicken (Gerade oder Kreis)")); }
    void onCancel() override { m_firstObject = nullptr; }
    bool hasIntermediateState() const override { return m_firstObject != nullptr; }

private:
    GeoObject* m_firstObject = nullptr;
};