#pragma once

#include <set>

#include "GeoObject.h"


class UpdateGuard {
public:
    UpdateGuard() { ++depth; };
    ~UpdateGuard();

    static void dequeue(GeoObject* obj) { pending().erase(obj); };
    static void enqueue(GeoObject* obj) { pending().insert(obj); };
    static void flush();
    static auto isActive() -> bool { return depth > 0 || flushing; };
    static auto isPending(GeoObject* obj) -> bool { return pending().contains(obj); };

private:
    static auto pending() -> std::set<GeoObject*>&;
    static int depth;
    static bool flushing;
};
