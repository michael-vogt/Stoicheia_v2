#include "HitTest.h"

#include "GeoCircleItem.h"
#include "GeoLinearObjectItem.h"
#include "GeoPointItem.h"

HitTest::HitTest(QGraphicsScene *scene, double tolerance) : m_scene(scene), m_tolerance(tolerance) {}

Point *HitTest::pointAt(const QPointF &scenePos) const {
    Point* nearest = nullptr;
    double minDist = m_tolerance * m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        if (const auto* pi = dynamic_cast<GeoPointItem*>(item)) {
            if (!pi->point()->isValid()) continue;
            const double dx = pi->point()->x() - scenePos.x();
            const double dy = pi->point()->y() - scenePos.y();
            const double d2 = dx*dx+dy*dy;
            if (d2 < minDist) {
                nearest = pi->point();
                minDist = d2;
            }
        }
    }

    return nearest;
}

LinearObject *HitTest::linearObjectAt(const QPointF &scenePos) const {
    LinearObject* nearest = nullptr;
    double minDist = m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        const auto* li = dynamic_cast<GeoLinearObjectItem*>(item);
        if (!li || !li->linearObject()->isValid()) continue;

        LinearObject* lo = li->linearObject();

        // Abstand von scenePos zur Trägergeraden
        double dx = lo->dx(), dy = lo->dy();
        double len = std::sqrt(dx*dx + dy*dy);
        if (len < 1e-10) continue;

        // Normalenvektor (normiert)
        double nx = -dy / len, ny = dx / len;

        // Vektor von p1 zum Punkt
        double vx = scenePos.x() - lo->p1()->x();
        double vy = scenePos.y() - lo->p1()->y();

        // Abstand zur Trägergeraden = Projektion auf Normalenvektor
        double dist = std::abs(vx * nx + vy * ny);

        // Für Ray und Segment: Parameter t prüfen
        double t = (vx*dx + vy*dy) / (len*len);
        if (!lo->containsParameter(t)) continue;

        if (dist < minDist) {
            nearest = lo;
            minDist = dist;
        }
    }

    return nearest;
}

Circle *HitTest::circleAt(const QPointF &scenePos) const {
    Circle* nearest = nullptr;
    double minDist = m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        const auto* ci = dynamic_cast<GeoCircleItem*>(item);
        if (!ci || !ci->circle()->isValid()) continue;;

        Circle* c = ci->circle();

        double dx = scenePos.x() - c->center()->x();
        double dy = scenePos.y() - c->center()->y();
        double dist = std::abs(std::sqrt(dx*dx + dy*dy) - c->radius());

        if (dist < minDist) {
            nearest = c;
            minDist = dist;
        }
    }

    return nearest;
}

GeoObject *HitTest::anyObjectAt(const QPointF &scenePos) const {
    if (Point* p = pointAt(scenePos)) return p;
    if (LinearObject* l = linearObjectAt(scenePos)) return l;
    if (Circle* c = circleAt(scenePos)) return c;
    return nullptr;
}
