#include "PngExporter.h"
#include "../../Constants.h"

#include <QPainter>


using namespace Constants::ExportConstants;

auto PngExporter::exportToFile(QGraphicsScene *scene, const QString &filename) -> bool {
    QRectF rect = scene->itemsBoundingRect();
    if (rect.isEmpty()) {
        m_lastError = tr("Keine sichtbaren Objekte");
        return false;
    }

    rect.adjust(-PNG::MARGIN, -PNG::MARGIN, PNG::MARGIN, PNG::MARGIN);

    double scale = m_dpi / PNG::DPI;
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
