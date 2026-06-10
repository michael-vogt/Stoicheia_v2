#pragma once

#include "Exporter.h"
#include "../../Constants.h"


using namespace Constants::ExportConstants;

class PngExporter : public Exporter {
    Q_OBJECT
    double m_dpi = PNG::DPI;
public:
    void setResolution(double dpi) { m_dpi = dpi; }
    auto exportToFile(QGraphicsScene* scene, const QString& filename) -> bool override;

    auto formatName() const -> QString override { return "PNG"; }
    auto fileExtension() const -> QString override { return "png"; }
    auto fileFilter() const -> QString override { return tr("PNG-Dateien (*.png)"); }
};