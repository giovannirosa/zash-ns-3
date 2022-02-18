#include <ctime>
#include <iostream>

using namespace std;

#include "enums_zash.h"

class Context {
   public:
    vector<int> accessWay;
    vector<int> localization;
    vector<int> group;
    vector<int> time;
    Context() {}
    Context(vector<int> aw, vector<int> loc, vector<int> g) {
        accessWay = aw;
        localization = loc;
        group = g;
    }
};

class Ontology {
   public:
    vector<int> userLevel;
    vector<int> deviceClass;
    vector<vector<int>> capabilities;
    Ontology(vector<int> ul, vector<int> dc, vector<vector<int>> c) {
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
    Request() {}
    Request(int i, Device d, User u, Context c, Action a) {
        id = i;
        device = d;
        user = u;
        context = c;
        action = a;
    }
};