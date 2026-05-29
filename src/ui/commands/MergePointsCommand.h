#pragma once
#include "Command.h"
#include "geometry/GeoObject.h"
#include "ui/SceneAdapter.h"


class MergePointsCommand : public Command {
    Q_OBJECT
    SceneAdapter* m_adapter;
    Point* m_survivor;
    Point* m_absorbed;
    double m_absorbedX = 0.0;
    double m_absorbedY = 0.0;

    std::vector<GeoObject*> m_rewired;

public:
    MergePointsCommand(SceneAdapter* adapter, Point* survivor, Point* absorbed);

    void execute() override;
    void undo() override;
    QString description() const override;

    Point* survivor() const { return m_survivor; }
    Point* absorbed() const { return m_absorbed; }
};
