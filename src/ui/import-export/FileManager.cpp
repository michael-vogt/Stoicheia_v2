#include "FileManager.h"
#include "Serializer.h"
#include "../../geometry/Scene.h"
#include "../SceneAdapter.h"
#include "../commands/CommandStack.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>

static const QString FILTER_JSON = QObject::tr("Stoicheia-Dateien (*.sto)");
static const QString FILTER_SVG  = QObject::tr("SVG-Dateien (*.svg)");

FileManager::FileManager(Scene* scene, SceneAdapter* adapter,
                          CommandStack* commandStack, QWidget* parent)
    : m_scene(scene), m_adapter(adapter),
      m_commandStack(commandStack), m_parent(parent)
{}

bool FileManager::newFile() {
    if (m_unsavedChanges) {
        auto btn = QMessageBox::question(m_parent,
            QObject::tr("Ungespeicherte Änderungen"),
            QObject::tr("Möchten Sie die aktuellen Änderungen speichern?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Cancel)  return false;
        if (btn == QMessageBox::Save && !save()) return false;
    }
    clearScene();
    m_currentFile.clear();
    m_unsavedChanges = false;
    updateTitle();
    return true;
}

bool FileManager::open() {
    if (m_unsavedChanges) {
        auto btn = QMessageBox::question(m_parent,
            QObject::tr("Ungespeicherte Änderungen"),
            QObject::tr("Möchten Sie die aktuellen Änderungen speichern?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (btn == QMessageBox::Cancel)  return false;
        if (btn == QMessageBox::Save && !save()) return false;
    }

    QString filename = QFileDialog::getOpenFileName(
        m_parent, QObject::tr("Datei öffnen"), {}, FILTER_JSON);
    if (filename.isEmpty()) return false;

    return loadFromFile(filename);
}

bool FileManager::save() {
    if (m_currentFile.isEmpty()) return saveAs();
    return saveToFile(m_currentFile);
}

bool FileManager::saveAs() {
    QString filename = QFileDialog::getSaveFileName(
        m_parent, QObject::tr("Datei speichern"), {}, FILTER_JSON);
    if (filename.isEmpty()) return false;
    if (!filename.endsWith(".sto")) filename += ".sto";
    return saveToFile(filename);
}

bool FileManager::exportSVG() const {
    QString filename = QFileDialog::getSaveFileName(
        m_parent, QObject::tr("SVG exportieren"), {}, FILTER_SVG);
    if (filename.isEmpty()) return false;
    if (!filename.endsWith(".svg")) filename += ".svg";

    Serializer s(m_scene, m_adapter);
    if (!s.exportSVG(filename)) {
        QMessageBox::critical(m_parent,
            QObject::tr("Fehler"), s.lastError());
        return false;
    }
    return true;
}

bool FileManager::saveToFile(const QString& filename) {
    Serializer s(m_scene, m_adapter);
    if (!s.save(filename)) {
        QMessageBox::critical(m_parent,
            QObject::tr("Fehler beim Speichern"), s.lastError());
        return false;
    }
    m_currentFile    = filename;
    m_unsavedChanges = false;
    updateTitle();
    return true;
}

bool FileManager::loadFromFile(const QString& filename) {
    clearScene();

    Serializer s(m_scene, m_adapter);
    if (!s.load(filename)) {
        QMessageBox::critical(m_parent,
            QObject::tr("Fehler beim Laden"), s.lastError());
        return false;
    }
    m_currentFile    = filename;
    m_unsavedChanges = false;
    updateTitle();
    return true;
}

void FileManager::clearScene() const {
    m_adapter->clear();
    m_scene->clear();
    m_commandStack->clear();
}

void FileManager::updateTitle() const {
    if (auto* w = qobject_cast<QMainWindow*>(m_parent)) {
        QString name = m_currentFile.isEmpty()
            ? QObject::tr("Unbenannt")
            : QFileInfo(m_currentFile).baseName();
        w->setWindowTitle(
            QString("Στοιχεῖα – %1%2")
                .arg(name)
                .arg(m_unsavedChanges ? " *" : ""));
    }
}
