#include "ConstructionTool.h"
#include "../DrawingBoard.h"
#include <QPen>
#include <QKeyEvent>

ConstructionTool::ConstructionTool(const ToolContext& ctx) : Tool(ctx) {}

void ConstructionTool::activate() {
    m_ctx.drawingBoard->viewport()->setCursor(cursor());
    m_ctx.drawingBoard->viewport()->setMouseTracking(true);
    onActivate();
}

void ConstructionTool::deactivate() {
    m_ctx.drawingBoard->viewport()->setMouseTracking(false);
    removePreview();
    onCancel();
    showStatus("");
}

void ConstructionTool::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (hasIntermediateState()) {
            removePreview();
            onCancel();
            onActivate(); // Status zurücksetzen
            event->accept();
        } else {
            // kein Zwischenstand -> ignorieren
            // DrawingBoard wechselt zum SelectTool
            event->ignore();
        }
        return;
    }
    event->ignore();
}

void ConstructionTool::showStatus(const QString& msg) {
    m_ctx.drawingBoard->showStatus(msg);
}

void ConstructionTool::setPreviewLine(const QLineF& line) {
    if (!m_previewLine) {
        m_previewLine = new QGraphicsLineItem();
        m_previewLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        m_ctx.drawingBoard->scene()->addItem(m_previewLine);
    }
    m_previewLine->setLine(line);
}

void ConstructionTool::setPreviewEllipse(const QRectF& rect) {
    if (!m_previewEllipse) {
        m_previewEllipse = new QGraphicsEllipseItem();
        m_previewEllipse->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        m_previewEllipse->setBrush(Qt::NoBrush);
        m_ctx.drawingBoard->scene()->addItem(m_previewEllipse);
    }
    m_previewEllipse->setRect(rect);
}

void ConstructionTool::removePreview() {
    clearHighlights();
    if (m_previewLine) {
        m_ctx.drawingBoard->scene()->removeItem(m_previewLine);
        delete m_previewLine;
        m_previewLine = nullptr;
    }
    if (m_previewEllipse) {
        m_ctx.drawingBoard->scene()->removeItem(m_previewEllipse);
        delete m_previewEllipse;
        m_previewEllipse = nullptr;
    }
}

void ConstructionTool::highlightObject(GeoObject *obj, bool on) {
    m_ctx.adapter->highlight(obj, on);
    if (on)
        m_highlighted.push_back(obj);
}

void ConstructionTool::clearHighlights() {
    for (GeoObject* obj : m_highlighted)
        m_ctx.adapter->highlight(obj, false);
    m_highlighted.clear();
}
