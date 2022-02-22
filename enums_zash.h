#include <map>
#include <vector>
using namespace std;

namespace enums {

enum Room { BEDROOM = 1,
            BATHROOM = 2,
            KITCHEN = 3,
            LIVINGROOM = 4,
            OFFICE = 5,
            HOUSE = 6,
            UNKOWN = 7 };

enum Time { MORNING = 1,
            AFTERNOON = 2,
            NIGHT = 3 };

class Enum {
   public:
    const char *key;
    int value;
    int weight;
    Enum() {}
    Enum(const char *k, int v, int w) {
        key = k;
        value = v;
        weight = w;
    }
};

static const map<const char *, Enum *> Action = {
    {"MANAGE", new Enum("MANAGE", 1, 40)},
    {"CONTROL", new Enum("CONTROL", 2, 20)},
    {"VIEW", new Enum("VIEW", 3, 0)}};

static const map<const char *, Enum *> UserLevel = {
    {"ADMIN", new Enum("ADMIN", 1, 70)},
    {"ADULT", new Enum("ADULT", 2, 50)},
    {"CHILD", new Enum("CHILD", 3, 30)},
    {"VISITOR", new Enum("VISITOR", 4, 0)}};

static const map<const char *, Enum *> DeviceClass = {
    {"CRITICAL", new Enum("CRITICAL", 1, 30)},
    {"NONCRITICAL", new Enum("NONCRITICAL", 2, 0)}};

static const map<const char *, Enum *> AccessWay = {
    {"REQUESTED", new Enum("REQUESTED", 1, 30)},
    {"HOUSE", new Enum("HOUSE", 2, 20)},
    {"PERSONAL", new Enum("PERSONAL", 3, 10)}};

static const map<const char *, Enum *> Localization = {
    {"INTERNAL", new Enum("INTERNAL", 1, 30)},
    {"EXTERNAL", new Enum("EXTERNAL", 2, 10)}};

static const map<const char *, Enum *> TimeClass = {
    {"COMMOM", new Enum("COMMOM", 1, 20)},
    {"UNCOMMOM", new Enum("UNCOMMOM", 2, 10)}};

static const map<const char *, Enum *> Age = {
    {"ADULT", new Enum("ADULT", 1, 30)},
    {"TEEN", new Enum("TEEN", 2, 20)},
    {"KID", new Enum("KID", 3, 10)}};

static const map<const char *, Enum *> Group = {
    {"TOGETHER", new Enum("TOGETHER", 1, 10)},
    {"ALONE", new Enum("ALONE", 2, 0)}};

};  // namespace enums