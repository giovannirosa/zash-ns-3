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
    enums::Enum userLevel;
    enums::Enum age;
    vector<string> rejected;
    time_t startInterval;
    bool blocked;
    User() {}
    User(int i, enums::Enum ul, enums::Enum a) {
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
    enums::Enum deviceClass;
    enums::Enum room;
    bool active;
    Device() {}
    Device(int i, string n, enums::Enum dc, enums::Enum r, bool a) {
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