#pragma once

#include "Exporter.h"
#include <vector>
#include <memory>
#include <QWidget>

class ExportManager {
    QGraphicsScene* m_scene;
    QWidget* m_parent;
    std::vector<std::unique_ptr<Exporter>> m_exporters;

public:
    ExportManager(QGraphicsScene* scene, QWidget* parent);

    // Alle Formate registrieren
    void registerExporter(std::unique_ptr<Exporter> exporter);

    // Zeigt Dateidialog mit allen verfügbaren Formaten
    bool exportWithDialog();

    // Direkt in ein bestimmtes Format exportieren
    bool exportAs(const QString& format, const QString& filename);
};