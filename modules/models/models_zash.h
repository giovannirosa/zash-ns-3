#ifndef MODELS_ZASH
#define MODELS_ZASH

#include <ctime>
#include <iostream>

using namespace std;

#include "enums_zash.h"

class Context {
   public:
    enums::Enum *accessWay;
    enums::Enum *localization;
    enums::Enum *group;
    enums::Enum *time;
    Context() {}
    Context(enums::Enum *aw, enums::Enum *loc, enums::Enum *g) {
        accessWay = aw;
        localization = loc;
        group = g;
    }
};

class Ontology {
   public:
    enums::Enum *userLevel;
    enums::Enum *deviceClass;
    vector<enums::Enum *> capabilities;
    Ontology(enums::Enum *ul, enums::Enum *dc, vector<enums::Enum *> c) {
        userLevel = ul;
        deviceClass = dc;
        capabilities = c;
    }
};

class User {
   public:
    int id;
    enums::Enum *userLevel;
    enums::Enum *age;
    vector<time_t> rejected;
    time_t startInterval;
    bool blocked;
    User() {}
    User(int i, enums::Enum *ul, enums::Enum *a) {
        id = i;
        userLevel = ul;
        age = a;
        rejected = {};
        startInterval = (time_t)(-1);
        blocked = false;
    }
};

class Device {
   public:
    int id;
    string name;
    enums::Enum *deviceClass;
    int room;
    bool active;
    Device() {}
    Device(int i, string n, enums::Enum *dc, int r, bool a) {
        id = i;
        name = n;
        deviceClass = dc;
        room = r;
        active = a;
    }
};

class Request {
   public:
    int id;
    Device device;
    User user;
    Context context;
    enums::Enum action;
    Request() {}
    Request(int i, Device d, User u, Context c, enums::Enum a) {
        id = i;
        device = d;
        user = u;
        context = c;
        action = a;
    }
};

#endif