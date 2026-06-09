#pragma once

#include "Exporter.h"

constexpr double DEFAULT_PNG_DPI = 96.0;
constexpr double DEFAULT_PNG_MARGIN = 20.0;

class PngExporter : public Exporter {
    Q_OBJECT
    double m_dpi = DEFAULT_PNG_DPI;
public:
    void setResolution(double dpi) { m_dpi = dpi; }
    auto exportToFile(QGraphicsScene* scene, const QString& filename) -> bool override;

    auto formatName() const -> QString override { return "PNG"; }
    auto fileExtension() const -> QString override { return "png"; }
    auto fileFilter() const -> QString override { return tr("PNG-Dateien (*.png)"); }
};