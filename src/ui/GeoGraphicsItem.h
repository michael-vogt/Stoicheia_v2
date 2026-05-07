#pragma once
#include <QGraphicsItem>
#include "../geometry/GeoObject.h"

// Abstrakte Basis für alle grafischen Geometrie-Objekte.
//
// Erbt von QGraphicsItem (Qt-Seite) UND von GeoObject (Geometrie-Seite),
// sodass es sich als Abhängiger im Dependency-Graph registrieren kann.
// Wenn das dahinterliegende Geometrieobjekt recompute() auslöst,
// wird dieses Objekt benachrichtigt und aktualisiert sich in Qt.

class GeoGraphicsItem : public QGraphicsItem, public GeoObject {
public:
    explicit GeoGraphicsItem(QGraphicsItem* parent = nullptr);

    // GeoObject::recompute() – wird aufgerufen wenn eine Quelle sich ändert.
    // Leitet die Änderung an Qt weiter.
    void recompute() override;

protected:
    // Unterklassen implementieren die eigentliche Geometrie-Aktualisierung.
    // Wird von recompute() aufgerufen, vor prepareGeometryChange().
    virtual void updateGeometry() = 0;
};
