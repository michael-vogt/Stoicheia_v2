#include <QApplication>

#include "geometry/geometry.h"
#include "constructions/constructions.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    Point* P1 = Scene::getInstance().create<Point>(-10,-10);
    Point* P2 = Scene::getInstance().create<Point>(10,10);
    Point* P3 = Scene::getInstance().create<Point>(-10,10);
    Point* P4 = Scene::getInstance().create<Point>(10,-10);
    Line* L1 = Scene::getInstance().create<Line>(P1, P2);
    Line* L2 = Scene::getInstance().create<Line>(P3, P4);

    LineLineIntersection* IL1L2 = Scene::getInstance().create<LineLineIntersection>(L1, L2);
    Circle* C = Scene::getInstance().create<Circle>(IL1L2->first(), 2.0);
    LineCircleIntersection* IL1C = Scene::getInstance().create<LineCircleIntersection>(L1, C);
    LineCircleIntersection* IL2C = Scene::getInstance().create<LineCircleIntersection>(L2, C);

    QApplication app(argc, argv);
    MainWindow window("Στοιχεῖα");
    return QApplication::exec();
}
