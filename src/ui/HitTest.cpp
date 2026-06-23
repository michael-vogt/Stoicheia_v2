#include "HitTest.h"

#include "GeoCircleItem.h"
#include "GeoLinearObjectItem.h"
#include "GeoPointItem.h"
#include <limits>


constexpr double eps = std::numeric_limits<double>::epsilon();

HitTest::HitTest(QGraphicsScene *scene, double tolerance) : m_scene(scene), m_tolerance(tolerance) {}

auto HitTest::pointAt(const QPointF &scenePos) const -> Point * {
    Point* nearest = nullptr;
    double min_dist = m_tolerance * m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        if (const auto* point_item = dynamic_cast<GeoPointItem*>(item)) {
            if (!point_item->point()->isValid()) {
                continue;
            }
            const double delta_x = point_item->point()->x() - scenePos.x();
            const double delta_y = point_item->point()->y() - scenePos.y();
            const double dist2 = (delta_x*delta_x)+(delta_y*delta_y);
            if (dist2 < min_dist) {
                nearest = point_item->point();
                min_dist = dist2;
            }
        }
    }

    return nearest;
}

auto HitTest::linearObjectAt(const QPointF &scenePos) const -> LinearObject * {
    LinearObject* nearest = nullptr;
    double min_dist = m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        const auto* linear_object_item = dynamic_cast<GeoLinearObjectItem*>(item);
        if ((linear_object_item == nullptr) || !linear_object_item->linearObject()->isValid()) {
            continue;
        }

        LinearObject* linear_object = linear_object_item->linearObject();

        // Abstand von scenePos zur Trägergeraden
        double delta_x = linear_object->dx();
        double delta_y = linear_object->dy();
        double len = std::sqrt((delta_x*delta_x) + (delta_y*delta_y));
        if (len < eps) {
            continue;
        }

        // Normalenvektor (normiert)
        double normal_x = -delta_y / len;
        double normal_y = delta_x / len;

        // Vektor von p1 zum Punkt
        double vec_x = scenePos.x() - linear_object->point1()->x();
        double vec_y = scenePos.y() - linear_object->point1()->y();

        // Abstand zur Trägergeraden = Projektion auf Normalenvektor
        double dist = std::abs((vec_x * normal_x) + (vec_y * normal_y));

        // Für Ray und Segment: Parameter t prüfen
        double param_t = ((vec_x*delta_x) + (vec_y*delta_y)) / (len*len);
        if (!linear_object->containsParameter(param_t)) {
            continue;
        }

        if (dist < min_dist) {
            nearest = linear_object;
            min_dist = dist;
        }
    }

    return nearest;
}

auto HitTest::circleAt(const QPointF &scenePos) const -> Circle * {
    Circle* nearest = nullptr;
    double min_dist = m_tolerance;

    const auto items = m_scene->items(
        QRectF(scenePos - QPointF(m_tolerance, m_tolerance),
            QSizeF(m_tolerance * 2, m_tolerance * 2)));

    for (QGraphicsItem* item : items) {
        const auto* circle_item = dynamic_cast<GeoCircleItem*>(item);
        if ((circle_item == nullptr) || !circle_item->circle()->isValid()) {
            continue;
        }

        Circle* circle = circle_item->circle();

        double delta_x = scenePos.x() - circle->center()->x();
        double delta_y = scenePos.y() - circle->center()->y();
        double dist = std::abs(std::sqrt((delta_x*delta_x) + (delta_y*delta_y)) - circle->radius());

        if (dist < min_dist) {
            nearest = circle;
            min_dist = dist;
        }
    }

    return nearest;
}

auto HitTest::anyObjectAt(const QPointF &scenePos) const -> GeoObject * {
    if (Point* point = pointAt(scenePos)) {
        return point;
    }
    if (LinearObject* linear_object = linearObjectAt(scenePos)) {
        return linear_object;
    }
    if (Circle* circle = circleAt(scenePos)) {
        return circle;
    }
    return nullptr;
}
