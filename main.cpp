#include <QApplication>
#include <qevent.h>

#include "geometry/geometry.h"
#include "constructions/constructions.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window("Στοιχεῖα");


    Point* P1 = window.geoScene()->create<Point>(-500,-500);
    Point* P2 = window.geoScene()->create<Point>(500,500);
    Point* P3 = window.geoScene()->create<Point>(-500,500);
    Point* P4 = window.geoScene()->create<Point>(500,-500);
    Line* L1 = window.geoScene()->create<Line>(P1, P2);
    Line* L2 = window.geoScene()->create<Line>(P3, P4);

    LineLineIntersection* IL1L2 = window.geoScene()->create<LineLineIntersection>(L1, L2);
    Circle* C = window.geoScene()->create<Circle>(IL1L2->first(), 200.0);
    LineCircleIntersection* IL1C = window.geoScene()->create<LineCircleIntersection>(L1, C);
    LineCircleIntersection* IL2C = window.geoScene()->create<LineCircleIntersection>(L2, C);

    window.adapter()->addPoint(P1);
    window.adapter()->addPoint(P2);
    window.adapter()->addPoint(P3);
    window.adapter()->addPoint(P4);

    window.adapter()->addLinearObject(L1);
    window.adapter()->addLinearObject(L2);

    window.adapter()->addIntersectionSet(IL1L2);
    window.adapter()->addIntersectionSet(IL1C);
    window.adapter()->addIntersectionSet(IL2C);
    window.adapter()->addCircle(C);


    /*auto* p1 = window.geoScene()->create<Point>(0, 0);
    auto* p2 = window.geoScene()->create<Point>(100, 100);
    window.adapter()->addPoint(p1);
    window.adapter()->addPoint(p2);
    window.adapter()->addLinearObject(
        window.geoScene()->create<Line>(p1, p2));*/


    window.showMaximized();
    /*window.showMinimized();
    window.showMaximized();*/

    /*P2->moveTo(1000, 500);
    P2->moveTo(1000, 250);*/

    return QApplication::exec();
}
