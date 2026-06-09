#include "ExportManager.h"

#include <QFileDialog>
#include <QMessageBox>

ExportManager::ExportManager(QGraphicsScene* scene, QWidget* parent)
    : m_scene(scene), m_parent(parent)
{}

void ExportManager::registerExporter(std::unique_ptr<Exporter> exporter) {
    m_exporters.push_back(std::move(exporter));
}

void ExportManager::exportWithDialog() const {
    // Filter aus allen Exportern zusammenbauen
    QStringList filters;
    for (const auto& exporter : m_exporters) {
        filters << exporter->fileFilter();
    }

    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(
        m_parent,
        tr("Exportieren"),
        {},
        filters.join(";;"),
        &selectedFilter);

    if (filename.isEmpty()) { 
        return;
    }

    // Passenden Exporter finden
    for (const auto& exporter : m_exporters) {
        if (selectedFilter == exporter->fileFilter()) {
            if (!filename.endsWith("." + exporter->fileExtension())) {
                filename += "." + exporter->fileExtension();
            }
            if (!exporter->exportToFile(m_scene, filename)) {
                QMessageBox::critical(m_parent,
                    tr("Fehler beim Export"), exporter->lastError());
                return;
            }
            return;
        }
    }
}
