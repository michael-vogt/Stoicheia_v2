#pragma once

#include "IntersectionSet.h"
#include "Structs.h"
#include "geometry/LinearObject.h"

class LineLineIntersection : public IntersectionSet {
public:
    LineLineIntersection(LinearObject* line1, LinearObject* line2);

    // Getter
    [[nodiscard]] auto line1() const -> LinearObject* { return m_line1; }
    [[nodiscard]] auto line2() const -> LinearObject* { return m_line2; }

    // Quellen aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void replaceSource(GeoObjectPair source) override;

protected:
    void compute() override;

private:
    LinearObject* m_line1;
    LinearObject* m_line2;
};