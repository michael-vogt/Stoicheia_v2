#include "SvgExporter.h"

#include <QPainter>
#include <QSvgGenerator>

#include "ui/GeoGraphicsItem.h"

bool SvgExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QRectF rect = scene->itemsBoundingRect();
    /*for (QGraphicsItem *item : scene->items()) {
        if (item->isVisible())
            rect = rect.united(item->sceneBoundingRect());
            //rect = rect.united(item->mapToScene(item->boundingRect()).boundingRect());
    }*/

    if (rect.isEmpty()) {
        m_lastError = QObject::tr("Keine sichtbaren Objekte");
        return false;
    }

    const double margin = 20.0;
    rect.adjust(-margin, -margin, margin, margin);

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
