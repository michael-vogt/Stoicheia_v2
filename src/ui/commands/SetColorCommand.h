#pragma once
#include "Command.h"
#include <QColor>

class SceneAdapter;
class GeoObject;

// Setzt die Farbe eines geometrischen Objekts (undo-fähig).
class SetColorCommand : public Command {
public:
    SetColorCommand(SceneAdapter* adapter, GeoObject* object, const QColor& newColor);

    void execute() override;
    void undo() override;
    [[nodiscard]] auto description() const -> QString override { return tr("Farbe setzen"); }

private:
    static void applyColor(SceneAdapter* adapter, GeoObject* obj, const QColor& color);

    SceneAdapter* m_adapter;
    GeoObject*    m_object;
    QColor        m_newColor;
    QColor        m_oldColor;
};