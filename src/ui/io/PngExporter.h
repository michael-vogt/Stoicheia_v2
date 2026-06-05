#pragma once

#include "Exporter.h"

class PngExporter : public Exporter {
    Q_OBJECT
    int m_dpi = 96;
public:
    void setResolution(int dpi) { m_dpi = dpi; }
    bool exportToFile(QGraphicsScene* scene, const QString& filename) override;

    QString formatName() const override { return "PNG"; }
    QString fileExtension() const override { return "png"; }
    QString fileFilter() const override { return tr("PNG-Dateien (*.png)"); }
};