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

    [[nodiscard]] auto geoObject() const -> GeoObject* { return m_object; }

    virtual auto contains(const QPointF& point) -> bool { return true; }

    // GeoObject::recompute() – wird aufgerufen wenn eine Quelle sich ändert.
    // Leitet die Änderung an Qt weiter.
    void recompute() override;
    void setGeoSelected(bool selected) { m_selected = selected; }
    [[nodiscard]] auto isGeoSelected() const -> bool { return m_selected; }

    void setHighlighted(bool highlighted);
    [[nodiscard]] auto isHighlighted() const -> bool { return m_highlighted; }

protected:
    bool m_selected = false;
    bool m_highlighted = false;
    GeoObject* m_object = nullptr;
    // Unterklassen implementieren die eigentliche Geometrie-Aktualisierung.
    // Wird von recompute() aufgerufen, vor prepareGeometryChange().
    virtual void updateGeometry() = 0;
};
