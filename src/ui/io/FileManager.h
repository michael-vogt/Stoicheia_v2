#pragma once
#include <QString>
#include <QWidget>

class Scene;
class SceneAdapter;
class CommandStack;

// Verwaltet Dateioperationen und den aktuellen Dateipfad.
class FileManager : public QObject {
    Q_OBJECT
public:
    FileManager(Scene* scene, SceneAdapter* adapter, CommandStack* commandStack, QWidget* parent);

    // Gibt false zurück wenn der Nutzer abbricht
    auto closeFile() -> bool;
    auto newFile() -> bool;
    auto open() -> bool;
    auto openFile(const QString& filename) -> bool;
    auto save() -> bool;
    auto saveAs() -> bool;
    [[nodiscard]] auto exportSVG() const -> bool;

    [[nodiscard]] auto hasUnsavedChanges() const -> bool { return m_unsavedChanges; }

    [[nodiscard]] auto currentFile() const -> QString { return m_currentFile; }

public slots: // NOLINT
        void markSaved()               { m_unsavedChanges = false; updateTitle(); }
        void markChanged()             { m_unsavedChanges = true;  updateTitle(); }

private:
    auto saveToFile(const QString& filename) -> bool;
    auto loadFromFile(const QString& filename) -> bool;
    void clearScene() const;
    void updateTitle() const;

    Scene*        m_scene;
    SceneAdapter* m_adapter;
    CommandStack* m_commandStack;
    QWidget*      m_parent;
    QString       m_currentFile;
    bool          m_unsavedChanges = false;
};