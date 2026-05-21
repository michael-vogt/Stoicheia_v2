#include "PngExporter.h"

#include <QPainter>
#include <QGraphicsItem>

bool PngExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QRectF rect; // = scene->sceneRect();
    for (QGraphicsItem* item : scene->items()) {
        if (item->isVisible()) {
            rect = rect.united(item->mapToScene(item->boundingRect()).boundingRect());
        }
    }

    if (rect.isEmpty()) {
        m_lastError = QObject::tr("Keine sichtbren Objekte");
        return false;
    }

    double margin = 20.0;
    rect.adjust(-margin, -margin, margin, margin);

    double scale = m_dpi / 96.0;
    QImage image((rect.size() * scale).toSize(), QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter, QRectF(), rect);
    painter.end();

    if (!image.save(filename)) {
        m_lastError = QObject::tr("Konnte Datei nicht schreiben");
        return false;
    }
    return true;
}
