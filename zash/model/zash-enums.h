#ifndef ENUMS_ZASH
#define ENUMS_ZASH

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace enums {

enum Room {
  BEDROOM = 1,
  BATHROOM = 2,
  KITCHEN = 3,
  LIVINGROOM = 4,
  OFFICE = 5,
  HOUSE = 6,
  UNKOWN = 7
};

enum Time { MORNING = 1, AFTERNOON = 2, NIGHT = 3 };

class Enum {
public:
  string key;
  int value;
  int weight;
  Enum();
  Enum(string k, int v, int w);
};

static const map<string, Enum *> Action = {
    {"MANAGE", new Enum("MANAGE", 1, 40)},
    {"CONTROL", new Enum("CONTROL", 2, 20)},
    {"VIEW", new Enum("VIEW", 3, 0)}};

static const map<string, Enum *> UserLevel = {
    {"ADMIN", new Enum("ADMIN", 1, 70)},
    {"ADULT", new Enum("ADULT", 2, 50)},
    {"CHILD", new Enum("CHILD", 3, 30)},
    {"VISITOR", new Enum("VISITOR", 4, 0)}};

static const map<string, Enum *> DeviceClass = {
    {"CRITICAL", new Enum("CRITICAL", 1, 30)},
    {"NONCRITICAL", new Enum("NONCRITICAL", 2, 0)}};

static const map<string, Enum *> AccessWay = {
    {"REQUESTED", new Enum("REQUESTED", 1, 30)},
    {"HOUSE", new Enum("HOUSE", 2, 20)},
    {"PERSONAL", new Enum("PERSONAL", 3, 10)}};

static const map<string, Enum *> Localization = {
    {"INTERNAL", new Enum("INTERNAL", 1, 30)},
    {"EXTERNAL", new Enum("EXTERNAL", 2, 10)}};

static const map<string, Enum *> TimeClass = {
    {"COMMON", new Enum("COMMON", 1, 20)},
    {"UNCOMMON", new Enum("UNCOMMON", 2, 10)}};

static const map<string, Enum *> Age = {{"ADULT", new Enum("ADULT", 1, 30)},
                                        {"TEEN", new Enum("TEEN", 2, 20)},
                                        {"KID", new Enum("KID", 3, 10)}};

static const map<string, Enum *> Group = {
    {"TOGETHER", new Enum("TOGETHER", 1, 10)},
    {"ALONE", new Enum("ALONE", 2, 0)}};

}; // namespace enums

#endif