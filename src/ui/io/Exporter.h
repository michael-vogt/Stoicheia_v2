#pragma once
#include <QString>
#include <QGraphicsScene>


class Exporter : public QObject {
public:
    virtual ~Exporter() = default;

    // Hauptmethode – gibt false + lastError() bei Fehler zurück
    virtual auto exportToFile(QGraphicsScene* scene, const QString&  filename) -> bool = 0;

    virtual auto formatName() const -> QString = 0; // "SVG", "PNG", ...
    virtual auto fileExtension() const -> QString = 0; // "svg", "png", ...
    virtual auto fileFilter() const -> QString = 0; // "SVG-Dateien (*.svg)"

    auto lastError() const -> QString { return m_lastError; }

protected:
    mutable QString m_lastError;
};
