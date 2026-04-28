#ifndef STOICHEIA_UPDATEGUARD_H
#define STOICHEIA_UPDATEGUARD_H
#include <unordered_set>

#include "GeoObject.h"


class UpdateGuard {
    static int depth;
    static std::unordered_set<GeoObject*> pending;
public:
    UpdateGuard();
    ~UpdateGuard();
    static void enqueue(GeoObject* obj);
    static void flush();
    static bool isActive();
    static bool isPending(GeoObject* obj);
};


#endif //STOICHEIA_UPDATEGUARD_H
