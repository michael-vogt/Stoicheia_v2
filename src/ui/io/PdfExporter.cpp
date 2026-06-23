#include "PdfExporter.h"

#include <QPainter>
#include <QPrinter>

auto PdfExporter::exportToFile(QGraphicsScene *scene, const QString &filename) -> bool {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageSize(QPageSize(scene->sceneRect().size(), QPageSize::Point));

    const QRectF page_rect = printer.pageRect(QPrinter::DevicePixel);
    const qreal height = page_rect.height();

    QPainter painter(&printer);
    painter.translate(0, height);
    painter.scale(1.0, -1.0);
    scene->render(&painter);
    return true;

}
