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
    FileManager(Scene* scene, SceneAdapter* adapter, CommandStack* commandStack,
                QWidget* parent);

    // Gibt false zurück wenn der Nutzer abbricht
    bool newFile();
    bool open();
    bool save();
    bool saveAs();
    bool exportSVG() const;

    bool hasUnsavedChanges() const { return m_unsavedChanges; }

    QString currentFile() const { return m_currentFile; }

public slots:
        void markSaved()               { m_unsavedChanges = false; updateTitle(); }
        void markChanged()             { m_unsavedChanges = true;  updateTitle(); }

private:
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);
    void clearScene() const;
    void updateTitle() const;

    Scene*        m_scene;
    SceneAdapter* m_adapter;
    CommandStack* m_commandStack;
    QWidget*      m_parent;
    QString       m_currentFile;
    bool          m_unsavedChanges = false;
};