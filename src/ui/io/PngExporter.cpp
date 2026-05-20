#include "PngExporter.h"

#include <QPainter>

bool PngExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QRectF rect = scene->sceneRect();
    double scale = m_dpi / 96.0;
    QImage image(rect.size().toSize() * scale, QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    painter.end();

    if (!image.save(filename)) {
        m_lastError = QObject::tr("Konnte Datei nicht schreiben");
        return false;
    }
    return true;
}
