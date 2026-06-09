#pragma once

#include "Exporter.h"
#include "../../Constants.h"

class PngExporter : public Exporter {
    Q_OBJECT
    double m_dpi = Constants::Export::PNG::DPI;
public:
    void setResolution(double dpi) { m_dpi = dpi; }
    auto exportToFile(QGraphicsScene* scene, const QString& filename) -> bool override;

    auto formatName() const -> QString override { return "PNG"; }
    auto fileExtension() const -> QString override { return "png"; }
    auto fileFilter() const -> QString override { return tr("PNG-Dateien (*.png)"); }
};