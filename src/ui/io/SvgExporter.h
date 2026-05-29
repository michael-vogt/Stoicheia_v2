#pragma once

#include "Exporter.h"

class SvgExporter : public Exporter {
    Q_OBJECT
public:
    bool exportToFile(QGraphicsScene* scene, const QString& filename) override;

    QString formatName() const override { return "SVG"; }
    QString fileExtension() const override { return "svg"; }
    QString fileFilter() const override { return tr("SVG-Dateien (*.svg)"); }
};