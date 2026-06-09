#pragma once

#include "Exporter.h"

class PdfExporter : public Exporter {
    Q_OBJECT
public:
    auto exportToFile(QGraphicsScene* scene, const QString& filename) -> bool override;

    auto formatName() const -> QString override { return "PDF"; }
    auto fileExtension() const -> QString override { return "pdf"; }
    auto fileFilter() const -> QString override { return tr("PDF-Dateien (*.pdf)"); }
};