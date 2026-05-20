#include "SvgExporter.h"

#include <QPainter>
#include <QSvgGenerator>

#include "ui/GeoGraphicsItem.h"

bool SvgExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QRectF rect; // = scene->sceneRect();
    for (QGraphicsItem *item : scene->items()) {
        if (item->isVisible())
            rect = rect.united(item->sceneBoundingRect());
            //rect = rect.united(item->mapToScene(item->boundingRect()).boundingRect());
    }

    if (rect.isEmpty()) {
        m_lastError = QObject::tr("Keine sichtbaren Objekte");
        return false;
    }

    const double margin = 20.0;
    rect.adjust(-margin, -margin, margin, margin);

    QSvgGenerator generator;
    generator.setFileName(filename);
    generator.setSize(rect.size().toSize());
    generator.setViewBox(rect);

    QPainter painter(&generator);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter, rect, rect);

    return true;
}
