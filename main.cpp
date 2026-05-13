#include <iostream>
#include <QApplication>
#include <qevent.h>
#include <QTranslator>

#include "geometry/geometry.h"
#include "constructions/constructions.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QTranslator translator;
    if (translator.load(":/i18n/app_en.qm")) {
        app.installTranslator(&translator);
    } else {
        std::cerr << "Error loading translator" << std::endl;
    }

    MainWindow window("Στοιχεῖα");

    DrawingBoard* board = window.drawingBoard();

    /*Point* P1 = board->geoScene()->create<Point>(-500,-500);
    Point* P2 = board->geoScene()->create<Point>(500,500);
    Point* P3 = board->geoScene()->create<Point>(-500,500);
    Point* P4 = board->geoScene()->create<Point>(500,-500);
    Line* L1 = board->geoScene()->create<Line>(P1, P2);
    Line* L2 = board->geoScene()->create<Line>(P3, P4);

    LineLineIntersection* IL1L2 = board->geoScene()->create<LineLineIntersection>(L1, L2);
    Circle* C = board->geoScene()->create<Circle>(IL1L2->first(), 200.0);
    LineCircleIntersection* IL1C = board->geoScene()->create<LineCircleIntersection>(L1, C);
    LineCircleIntersection* IL2C = board->geoScene()->create<LineCircleIntersection>(L2, C);

    board->adapter()->addPoint(P1);
    board->adapter()->addPoint(P2);
    board->adapter()->addPoint(P3);
    board->adapter()->addPoint(P4);

    board->adapter()->addLinearObject(L1);
    board->adapter()->addLinearObject(L2);

    board->adapter()->addIntersectionSet(IL1L2);

    /*board->adapter()->addIntersectionSet(IL1C);
    board->adapter()->addIntersectionSet(IL2C);
    board->adapter()->addCircle(C);*/

    window.showMaximized();

    return QApplication::exec();
}
