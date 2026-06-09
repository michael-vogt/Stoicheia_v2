#include "SvgExporter.h"

#include <QPainter>
#include <QSvgGenerator>

auto SvgExporter::exportToFile(QGraphicsScene *scene, const QString &filename) -> bool {
    QRectF rect = scene->itemsBoundingRect();
    if (rect.isEmpty()) {
        m_lastError = tr("Keine sichtbaren Objekte");
        return false;
    }

    rect.adjust(-DEFAULT_SVG_MARGIN, -DEFAULT_SVG_MARGIN, DEFAULT_SVG_MARGIN, DEFAULT_SVG_MARGIN);

    QSvgGenerator generator;
    generator.setFileName(filename);
    generator.setSize(rect.size().toSize());
    generator.setViewBox(QRectF(QPointF(0,0), rect.size()));

    QPainter painter(&generator);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(0, rect.height());
    painter.scale(1.0, -1.0);
    painter.translate(-rect.left(), -rect.top());

    scene->render(&painter, rect, rect);

    return true;
}
