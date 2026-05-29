#include "ExportManager.h"

#include <QFileDialog>
#include <QMessageBox>

ExportManager::ExportManager(QGraphicsScene* scene, QWidget* parent)
    : m_scene(scene), m_parent(parent)
{}

void ExportManager::registerExporter(std::unique_ptr<Exporter> exporter) {
    m_exporters.push_back(std::move(exporter));
}

bool ExportManager::exportWithDialog() {
    // Filter aus allen Exportern zusammenbauen
    QStringList filters;
    for (auto& e : m_exporters)
        filters << e->fileFilter();

    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(
        m_parent,
        tr("Exportieren"),
        {},
        filters.join(";;"),
        &selectedFilter);

    if (filename.isEmpty()) return false;

    // Passenden Exporter finden
    for (auto& e : m_exporters) {
        if (selectedFilter == e->fileFilter()) {
            if (!filename.endsWith("." + e->fileExtension()))
                filename += "." + e->fileExtension();
            if (!e->exportToFile(m_scene, filename)) {
                QMessageBox::critical(m_parent,
                    tr("Fehler beim Export"), e->lastError());
                return false;
            }
            return true;
        }
    }
    return false;
}