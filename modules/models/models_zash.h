#ifndef MODELS_ZASH
#define MODELS_ZASH

#include <ctime>
#include <iostream>

using namespace std;

#include "enums_zash.h"
#include "utils.h"

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

    friend ostream &operator<<(ostream &out, Context const &c) {
        out << "Context["
            << c.accessWay->key << ","
            << c.localization->key << ","
            << c.time->key << ","
            << c.group->key << "]";
        return out;
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

    friend ostream &operator<<(ostream &out, Ontology const &o) {
        out << "Ontology["
            << o.userLevel->key << ","
            << o.deviceClass->key << ","
            << vecToStr(o.capabilities) << "]";
        return out;
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

    friend ostream &operator<<(ostream &out, User const &u) {
        out << "User["
            << u.id << ","
            << u.userLevel->key << ","
            << u.age->key << "]";
        return out;
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

    friend ostream &operator<<(ostream &out, Device const &d) {
        out << "Device["
            << d.id << ","
            << d.name << ","
            << d.deviceClass->key << ","
            << d.room << "]";
        return out;
    }
};

class Request {
   public:
    int id;
    Device *device;
    User *user;
    Context *context;
    enums::Enum *action;
    Request() {}
    Request(int i, Device *d, User *u, Context *c, enums::Enum *a) {
        id = i;
        device = d;
        user = u;
        context = c;
        action = a;
    }

    friend ostream &operator<<(ostream &out, Request const &r) {
        out << "Request["
            << r.id << ","
            << r.device << ","
            << r.user << ","
            << r.context << ","
            << r.action->key << "]";
        return out;
    }
};

#endif