#pragma once

#include "../geometry/GeoObject.h"
#include "../geometry/Line.h"
#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"
#include "Structs.h"


class Perpendicular : public GeoObject {
public:
    Perpendicular(Point* origin, LinearObject* reference);

    [[nodiscard]] auto origin() const -> Point* { return m_origin; }
    [[nodiscard]] auto reference() const -> LinearObject* { return m_reference; }
    auto line() -> LinearObject* { return m_line; };

    void onSourceRemoved(GeoObject* src) override;
    void recompute() override;
    void replaceSource(GeoObjectPair source) override;

private:
    // Komponenten des Normalenvektors der Referenzlinie
    [[nodiscard]] auto px() const -> double;
    [[nodiscard]] auto py() const -> double;

    Point* m_origin;
    LinearObject* m_reference;
    Point m_phantom;
    Line* m_line = nullptr;
};