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
        removePreview();
        onCancel();
        onActivate(); // Status zurücksetzen
        event->accept();
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