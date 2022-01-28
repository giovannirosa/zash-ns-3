#include <vector>
using namespace std;

enum class Room { BEDROOM = 1,
                  BATHROOM = 2,
                  KITCHEN = 3,
                  LIVINGROOM = 4,
                  OFFICE = 5,
                  HOUSE = 6,
                  UNKOWN = 7 };

enum class Time { MORNING = 1,
                  AFTERNOON = 2,
                  NIGHT = 3 };

class Action {
   public:
    vector<int> MANAGE = {1, 40},
                CONTROL = {2, 20},
                VIEW = {3, 0};
};

class DeviceClass {
   public:
    vector<int> CRITICAL = {1, 30},
                NONCRITICAL = {2, 0};
};

class UserLevel {
   public:
    vector<int> ADMIN = {1, 70},
                ADULT = {2, 50},
                CHILD = {3, 30},
                VISITOR = {4, 0};
};

class AccessWay {
   public:
    vector<int> REQUESTED = {1, 30},
                HOUSE = {2, 20},
                PERSONAL = {3, 10};
};

class Localization {
   public:
    vector<int> INTERNAL = {1, 30},
                EXTERNAL = {2, 10};
};

class TimeClass {
   public:
    vector<int> COMMOM = {1, 20},
                UNCOMMOM = {2, 10};
};

class Age {
   public:
    vector<int> ADULT = {1, 30},
                TEEN = {2, 20},
                KID = {2, 10};
};

class Group {
   public:
    vector<int> TOGETHER = {1, 10},
                ALONE = {2, 0};
};