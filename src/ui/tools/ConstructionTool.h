#pragma once
#include "Tool.h"
#include <QGraphicsEllipseItem>

class ConstructionTool : public Tool {
public:
    explicit ConstructionTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void keyPressEvent(QKeyEvent* event) override;
    [[nodiscard]] auto cursor() const -> QCursor override { return Qt::CrossCursor; }

protected:
    // Unterklassen implementieren diese Methoden
    virtual void onActivate() {}
    virtual void onCancel()   {}

    // Statusmeldung anzeigen
    void showStatus(const QString& msg) const;

    // Vorschau-Hilfsmittel
    void setPreviewLine(const QLineF& line);
    void setPreviewEllipse(const QRectF& rect);
    void removePreview();

    void highlightObject(GeoObject* obj, bool isHighlighted);
    void clearHighlights();
    [[nodiscard]] virtual auto hasIntermediateState() const -> bool = 0;

private:
    QGraphicsLineItem* m_previewLine = nullptr;
    QGraphicsEllipseItem* m_previewEllipse = nullptr;
    std::vector<GeoObject*> m_highlighted;
};