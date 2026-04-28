#include <iostream>

#include "src/geometry/Line.h"
#include "src/geometry/Point.h"
#include "src/geometry/Scene.h"

int main() {
    Scene scene;

    Point p1(0, 0);
    Point p2(3, 4);
    Line L1(&p1, &p2);
    Line L2(1, 2, 3);

    std::cout << L2.toString() << std::endl;

    std::cout << L1.toString() << std::endl;
    p2.moveTo(6, 9);
    p1.moveTo(0, 1);
    std::cout << L1.toString() << std::endl;

    return 0;
}
