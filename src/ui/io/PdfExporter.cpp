#include "PdfExporter.h"

#include <QPainter>
#include <QPrinter>

bool PdfExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageSize(QPageSize(scene->sceneRect().size(), QPageSize::Point));

    const QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
    const qreal h = pageRect.height();

    QPainter painter(&printer);
    painter.translate(0, h);
    painter.scale(1.0, -1.0);
    scene->render(&painter);
    return true;

}
