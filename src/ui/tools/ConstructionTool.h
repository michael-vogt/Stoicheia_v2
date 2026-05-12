#pragma once
#include "Tool.h"
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QString>

// Abstrakte Basis für alle Konstruktions-Tools.
// Verwaltet gemeinsame Logik:
//   - Statusmeldungen für jeden Schritt
//   - Vorschau-Objekte
//   - Escape bricht ab und räumt auf
class ConstructionTool : public Tool {
    QGraphicsLineItem* m_previewLine = nullptr;
    QGraphicsEllipseItem* m_previewEllipse = nullptr;

protected:
    // Unterklassen implementieren diese Methoden
    virtual void onActivate() {}
    virtual void onCancel()   {}

    // Statusmeldung anzeigen
    void showStatus(const QString& msg);

    // Vorschau-Hilfsmittel
    void setPreviewLine(const QLineF& line);
    void setPreviewEllipse(const QRectF& rect);
    void removePreview();

public:
    explicit ConstructionTool(const ToolContext& ctx);

    void activate() override;
    void deactivate() override;

    void keyPressEvent(QKeyEvent* event) override;
    QCursor cursor() const override { return Qt::CrossCursor; }
};