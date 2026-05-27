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
    explicit GeoGraphicsItem(GeoObject* object, QGraphicsItem* parent = nullptr);

    GeoObject* geoObject() const { return m_object; }

    virtual bool contains(const QPointF& point) { return true; }

    // GeoObject::recompute() – wird aufgerufen wenn eine Quelle sich ändert.
    // Leitet die Änderung an Qt weiter.
    void recompute() override;
    void setGeoSelected(bool selected) { m_selected = selected; }
    bool isGeoSelected() const { return m_selected; }

    void setHighlighted(bool highlighted);
    bool isHighlighted() const { return m_highlighted; }

    bool equals(const GeoObject &other) const override { return false; }

protected:
    bool m_selected = false;
    bool m_highlighted = false;
    GeoObject* m_object = nullptr;
    // Unterklassen implementieren die eigentliche Geometrie-Aktualisierung.
    // Wird von recompute() aufgerufen, vor prepareGeometryChange().
    virtual void updateGeometry() = 0;
};
