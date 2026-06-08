#pragma once

#include <unordered_set>

#include "GeoObject.h"


class UpdateGuard {
public:
    UpdateGuard() { ++depth; };
    ~UpdateGuard() { if (--depth == 0) { flush(); } };

    static void dequeue(GeoObject* obj) { pending.erase(obj); };
    static void enqueue(GeoObject* obj) { pending.insert(obj); };
    static void flush();
    static auto isActive() -> bool { return depth > 0 || flushing; };
    static auto isPending(GeoObject* obj) -> bool { return pending.contains(obj); };

private:
    static int depth;
    static bool flushing;
    static std::unordered_set<GeoObject*> pending;
};
