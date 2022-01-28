#include <iostream>
#include <ctime>

using namespace std;

#include "enums_zash.h"

class Context {
   public:
    AccessWay accessWay;
    Localization localization;
    Group group;
    TimeClass time;
    Context() {}
    Context(AccessWay aw, Localization loc, Group g) {
        accessWay = aw;
        localization = loc;
        group = g;
    }
};

class Ontology {
   public:
    UserLevel userLevel;
    DeviceClass deviceClass;
    vector<Action> capabilities;
    Ontology(UserLevel ul, DeviceClass dc, vector<Action> c) {
        userLevel = ul;
        deviceClass = dc;
        capabilities = c;
    }
};

class User {
   public:
    int id;
    UserLevel userLevel;
    Age age;
    vector<string> rejected;
    time_t startInterval;
    bool blocked;
    User() {}
    User(int i, UserLevel ul, Age a) {
        id = i;
        userLevel = ul;
        age = a;
        rejected = {};
        startInterval = NULL;
        blocked = false;
    }
};

class Device {
   public:
    int id;
    string name;
    DeviceClass deviceClass;
    Room room;
    bool active;
    Device() {}
    Device(int i, string n, DeviceClass dc, Room r, bool a) {
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
    Action action;
    Request(int i, Device d, User u, Context c, Action a) {
        id = i;
        device = d;
        user = u;
        context = c;
        action = a;
    }
};