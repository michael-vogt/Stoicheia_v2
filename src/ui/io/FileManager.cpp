#include "FileManager.h"
#include "Serializer.h"
#include "../../geometry/Scene.h"
#include "../SceneAdapter.h"
#include "../commands/CommandStack.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>

static const QString FILTER_JSON = FileManager::tr("Stoicheia-Dateien (*.sto)");
static const QString FILTER_SVG  = FileManager::tr("SVG-Dateien (*.svg)");

FileManager::FileManager(Scene* scene, SceneAdapter* adapter,
                          CommandStack* commandStack, QWidget* parent)
    : m_scene(scene), m_adapter(adapter),
      m_commandStack(commandStack), m_parent(parent)
{}

auto FileManager::newFile() -> bool {
    if (m_unsavedChanges) {
        auto btn = QMessageBox::question(m_parent,
            tr("Ungespeicherte Änderungen"),
            tr("Möchten Sie die aktuellen Änderungen speichern?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Cancel)  {
            return false;
        }
        if (btn == QMessageBox::Save && !save()) {
            return false;
        }
    }
    clearScene();
    m_currentFile.clear();
    m_unsavedChanges = false;
    updateTitle();
    return true;
}

auto FileManager::open() -> bool {
    if (m_unsavedChanges) {
        auto btn = QMessageBox::question(m_parent,
            tr("Ungespeicherte Änderungen"),
            tr("Möchten Sie die aktuellen Änderungen speichern?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Cancel) {
            return false;
        }
        if (btn == QMessageBox::Save && !save()) {
            return false;
        }
    }

    QString filename = QFileDialog::getOpenFileName(
        m_parent, tr("Datei öffnen"), {}, FILTER_JSON);
    if (filename.isEmpty()) {
        return false;
    }

    return loadFromFile(filename);
}

auto FileManager::openFile(const QString& filename) -> bool {
    if (m_unsavedChanges) {
        auto btn = QMessageBox::question(m_parent,
            tr("Ungespeicherte Änderungen"),
            tr("Möchten Sie die aktuellen Änderungen speichern?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (btn == QMessageBox::Cancel) {
            return false;
        }
        if (btn == QMessageBox::Save && !save()) {
            return false;
        }
    }
    return loadFromFile(filename);

}

auto FileManager::save() -> bool {
    if (m_currentFile.isEmpty()) {
        return saveAs();
    }
    return saveToFile(m_currentFile);
}

auto FileManager::saveAs() -> bool {
    QString filename = QFileDialog::getSaveFileName(
        m_parent, tr("Datei speichern"), {}, FILTER_JSON);
    if (filename.isEmpty()) {
        return false;
    }
    if (!filename.endsWith(".sto")) {
        filename += ".sto";
    }
    return saveToFile(filename);
}

auto FileManager::exportSVG() const -> bool {
    QString filename = QFileDialog::getSaveFileName(
        m_parent, tr("SVG exportieren"), {}, FILTER_SVG);
    if (filename.isEmpty()) {
        return false;
    }
    if (!filename.endsWith(".svg")) {
        filename += ".svg";
    }

    Serializer serializer(m_scene, m_adapter);
    if (!serializer.exportSVG(filename)) {
        QMessageBox::critical(m_parent,
            tr("Fehler"), serializer.lastError());
        return false;
    }
    return true;
}

auto FileManager::saveToFile(const QString& filename) -> bool {
    Serializer serializer(m_scene, m_adapter);
    if (!serializer.save(filename)) {
        QMessageBox::critical(m_parent,
            tr("Fehler beim Speichern"), serializer.lastError());
        return false;
    }
    m_currentFile    = filename;
    m_unsavedChanges = false;
    AppSettings::instance().addRecentFile(filename);
    updateTitle();
    return true;
}

auto FileManager::loadFromFile(const QString& filename) -> bool {
    clearScene();

    Serializer serializer(m_scene, m_adapter);
    if (!serializer.load(filename)) {
        QMessageBox::critical(m_parent,
            tr("Fehler beim Laden"), serializer.lastError());
        return false;
    }
    m_currentFile    = filename;
    m_unsavedChanges = false;
    AppSettings::instance().addRecentFile(filename);
    updateTitle();
    return true;
}

void FileManager::clearScene() const {
    m_adapter->clear();
    m_scene->clear();
    m_scene->clearGraveyard();
    m_commandStack->clear();
}

void FileManager::updateTitle() const {
    if (auto* wnd = qobject_cast<QMainWindow*>(m_parent)) {
        QString name = m_currentFile.isEmpty()
            ? tr("Unbenannt")
            : QFileInfo(m_currentFile).baseName();
        wnd->setWindowTitle(
            QString(tr("Stoicheia (Στοιχεῖα) – %1%2"))
                .arg(name)
                .arg(m_unsavedChanges ? " *" : ""));
    }
}
