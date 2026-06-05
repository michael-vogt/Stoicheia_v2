#include "PngExporter.h"

#include <QPainter>

bool PngExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QRectF rect = scene->itemsBoundingRect();
    if (rect.isEmpty()) {
        m_lastError = tr("Keine sichtbaren Objekte");
        return false;
    }

    constexpr double margin = 20.0;
    rect.adjust(-margin, -margin, margin, margin);

    double scale = m_dpi / 96.0;
    QImage image((rect.size() * scale).toSize(),
                  QImage::Format_ARGB32);
    image.fill(Qt::white);

    // Erst normal rendern
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter, QRectF(QPointF(0,0), image.size()), rect);
    painter.end();

    // Dann Y-Flip auf das fertige Image anwenden
    //image = image.mirrored(false, true);
    image = image.flipped(Qt::Vertical);

    if (!image.save(filename)) {
        m_lastError = tr("Konnte Datei nicht schreiben");
        return false;
    }
    return true;
}
