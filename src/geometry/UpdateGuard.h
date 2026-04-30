#ifndef STOICHEIA_UPDATEGUARD_H
#define STOICHEIA_UPDATEGUARD_H
#include <unordered_set>

#include "GeoObject.h"


class UpdateGuard {
    static int depth;
    static bool flushing;
    static std::unordered_set<GeoObject*> pending;
public:
    UpdateGuard() { ++depth; };
    ~UpdateGuard() { if (--depth == 0) flush(); };
    static void enqueue(GeoObject* obj) { pending.insert(obj); };
    static void dequeue(GeoObject* obj) { pending.erase(obj); };
    static void flush();
    static bool isActive() { return depth > 0 || flushing; };
    static bool isPending(GeoObject* obj) { return pending.contains(obj); };
};


#endif //STOICHEIA_UPDATEGUARD_H
