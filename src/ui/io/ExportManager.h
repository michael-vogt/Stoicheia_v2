#pragma once

#include "Exporter.h"
#include <vector>
#include <memory>
#include <QWidget>

class ExportManager : QObject {

    Q_OBJECT

public:
    ExportManager(QGraphicsScene* scene, QWidget* parent);

    // Alle Formate registrieren
    void registerExporter(std::unique_ptr<Exporter> exporter);

    // Zeigt Dateidialog mit allen verfügbaren Formaten
    void exportWithDialog() const;

private:
    QGraphicsScene* m_scene;
    QWidget* m_parent;
    std::vector<std::unique_ptr<Exporter>> m_exporters;
};