#pragma once

class CreatePointCommand;

struct CreatePointCommandPairForCircle {
    CreatePointCommand* centerPointCmd;
    CreatePointCommand* radiusPointCmd;
};

struct CreatePointCommandPairForLinearObject {
    CreatePointCommand* firstPoint;
    CreatePointCommand* secondPoint;
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

struct PointPairForMergePointCommand {
    Point* survivor;
    Point* absorbed;
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

struct IntPair {
    int width;
    int height;
};

struct ToolTypePair {
    int toolType;
    int subType;
};