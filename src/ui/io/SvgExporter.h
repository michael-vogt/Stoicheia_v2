#pragma once

#include "Exporter.h"

class SvgExporter : public Exporter {
public:
    bool exportToFile(QGraphicsScene* scene, const QString& filename) override;

    QString formatName() const override { return "SVG"; }
    QString fileExtension() const override { return "svg"; }
    QString fileFilter() const override { return QObject::tr("SVG-Dateien (*.svg)"); }
};