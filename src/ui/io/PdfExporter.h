#pragma once

#include "Exporter.h"

class PdfExporter : public Exporter {
public:
    bool exportToFile(QGraphicsScene* scene, const QString& filename) override;

    QString formatName() const override { return "PDF"; }
    QString fileExtension() const override { return "pdf"; }
    QString fileFilter() const override { return QObject::tr("PDF-Dateien (*.pdf)"); }
};