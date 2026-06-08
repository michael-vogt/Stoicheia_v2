#pragma once

class CreatePointCommand;

struct CreatePointCommandPair {
    CreatePointCommand* centerPointCmd;
    CreatePointCommand* radiusPointCmd;
};

class Point;

struct PointPairForLinearObject {
    Point* point1;
    Point* point2;
};

struct PointPairForCircle {
    Point* center;
    Point* radiusPoint;
};

class GeoObject;

struct GeoObjectPair {
    GeoObject* oldGeoObject;
    GeoObject* newGeoObject;
};

struct DoublePair {
    double x;
    double y;
};