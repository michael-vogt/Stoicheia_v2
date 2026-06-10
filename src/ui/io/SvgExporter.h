#pragma once

#include "Exporter.h"


class SvgExporter : public Exporter {
    Q_OBJECT
public:
    auto exportToFile(QGraphicsScene* scene, const QString& filename) -> bool override;

    auto formatName() const -> QString override { return "SVG"; }
    auto fileExtension() const -> QString override { return "svg"; }
    auto fileFilter() const -> QString override { return tr("SVG-Dateien (*.svg)"); }
};