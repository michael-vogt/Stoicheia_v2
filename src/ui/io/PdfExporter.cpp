#include "PdfExporter.h"

#include <QPainter>
#include <QPrinter>

bool PdfExporter::exportToFile(QGraphicsScene *scene, const QString &filename) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageSize(QPageSize(scene->sceneRect().size(), QPageSize::Point));

    QPainter painter(&printer);
    scene->render(&painter);
    return true;

}
