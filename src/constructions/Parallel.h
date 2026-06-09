#pragma once

#include "../geometry/GeoObject.h"
#include "../geometry/Line.h"
#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"
#include "Structs.h"
#include <cstddef>


class Parallel : public GeoObject {
public:
    Parallel(Point* origin, LinearObject* reference);

    // Getter
    [[nodiscard]] auto origin() const -> Point* { return m_origin; }
    [[nodiscard]] auto reference() const -> LinearObject* { return m_reference; }
    auto line() -> LinearObject* { return m_line; }

    // Objekt aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObjectPair source) override;

private:
    // Steigung der Referenzlinie
    [[nodiscard]] auto dx() const -> double;
    [[nodiscard]] auto dy() const -> double;

    Point* m_origin;
    Point m_phantom;
    LinearObject* m_reference;
    Line* m_line = nullptr;
};