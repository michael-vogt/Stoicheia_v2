#pragma once
#include <QString>
#include <QGraphicsScene>


class Exporter : public QObject {
public:
    virtual ~Exporter() = default;

    // Hauptmethode – gibt false + lastError() bei Fehler zurück
    virtual bool exportToFile(QGraphicsScene* scene, const QString&  filename) = 0;

    virtual QString formatName() const = 0; // "SVG", "PNG", ...
    virtual QString fileExtension() const = 0; // "svg", "png", ...
    virtual QString fileFilter() const = 0; // "SVG-Dateien (*.svg)"

    QString lastError() const { return m_lastError; }

protected:
    mutable QString m_lastError;
};
