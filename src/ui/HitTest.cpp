#include "HitTest.h"

#include "GeoCircleItem.h"
#include "GeoLinearObjectItem.h"
#include "GeoPointItem.h"
#include <limits>


constexpr double eps = std::numeric_limits<double>::epsilon();

HitTest::HitTest(QGraphicsScene *scene, double tolerance) : m_scene(scene), m_tolerance(tolerance) {}

auto HitTest::pointAt(const QPointF &scenePos) const -> Point * {
    Point* nearest = nullptr;
    double minDist = m_tolerance * m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        if (const auto* pointItem = dynamic_cast<GeoPointItem*>(item)) {
            if (!pointItem->point()->isValid()) {
                continue;
            }
            const double delta_x = pointItem->point()->x() - scenePos.x();
            const double delta_y = pointItem->point()->y() - scenePos.y();
            const double dist2 = (delta_x*delta_x)+(delta_y*delta_y);
            if (dist2 < minDist) {
                nearest = pointItem->point();
                minDist = dist2;
            }
        }
    }

    return nearest;
}

auto HitTest::linearObjectAt(const QPointF &scenePos) const -> LinearObject * {
    LinearObject* nearest = nullptr;
    double minDist = m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        const auto* linearObjectItem = dynamic_cast<GeoLinearObjectItem*>(item);
        if ((linearObjectItem == nullptr) || !linearObjectItem->linearObject()->isValid()) {
            continue;
        }

        LinearObject* linearObject = linearObjectItem->linearObject();

        // Abstand von scenePos zur Trägergeraden
        double delta_x = linearObject->dx();
        double delta_y = linearObject->dy();
        double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
        if (len < eps) {
            continue;
        }

        // Normalenvektor (normiert)
        double normal_x = -delta_y / len;
        double normal_y = delta_x / len;

        // Vektor von p1 zum Punkt
        double vec_x = scenePos.x() - linearObject->point1()->x();
        double vec_y = scenePos.y() - linearObject->point1()->y();

        // Abstand zur Trägergeraden = Projektion auf Normalenvektor
        double dist = std::abs((vec_x * normal_x) + (vec_y * normal_y));

        // Für Ray und Segment: Parameter t prüfen
        double param_t = ((vec_x*delta_x) + (vec_y*delta_y)) / (len*len);
        if (!linearObject->containsParameter(param_t)) {
            continue;
        }

        if (dist < minDist) {
            nearest = linearObject;
            minDist = dist;
        }
    }

    return nearest;
}

auto HitTest::circleAt(const QPointF &scenePos) const -> Circle * {
    Circle* nearest = nullptr;
    double minDist = m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        const auto* circleItem = dynamic_cast<GeoCircleItem*>(item);
        if ((circleItem == nullptr) || !circleItem->circle()->isValid()) {
            continue;
        }

        Circle* circle = circleItem->circle();

        double delta_x = scenePos.x() - circle->center()->x();
        double delta_y = scenePos.y() - circle->center()->y();
        double dist = std::abs(std::sqrt((delta_x*delta_x) + (delta_y*delta_y)) - circle->radius());

        if (dist < minDist) {
            nearest = circle;
            minDist = dist;
        }
    }

    return nearest;
}

auto HitTest::anyObjectAt(const QPointF &scenePos) const -> GeoObject * {
    if (Point* point = pointAt(scenePos)) {
        return point;
    }
    if (LinearObject* linearObject = linearObjectAt(scenePos)) {
        return linearObject;
    }
    if (Circle* circle = circleAt(scenePos)) {
        return circle;
    }
    return nullptr;
}
