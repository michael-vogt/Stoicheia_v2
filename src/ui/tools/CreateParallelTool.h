#pragma once
#include "ConstructionTool.h"
#include "geometry/LinearObject.h"

class CreateParallelTool : public ConstructionTool {

    Q_OBJECT

public:
    explicit CreateParallelTool(const ToolContext& ctx);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

protected:
    void onActivate() override { showStatus(tr("Referenzgerade klicken")); }
    void onCancel() override { m_reference = nullptr; }
    [[nodiscard]] auto hasIntermediateState() const -> bool override { return m_reference != nullptr; }

private:
    LinearObject* m_reference = nullptr;
};