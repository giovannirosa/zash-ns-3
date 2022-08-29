#ifndef ENUMS_ZASH
#define ENUMS_ZASH

#include <map>
#include <vector>
#include <string>
#include <iostream>
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

class Enum
{
public:
  string key;
  int value;
  int weight;
  Enum ();
  Enum (string k, int v, int w);

  friend ostream &
  operator<< (ostream &out, Enum const &e)
  {
    out << "" << e.key << "[" << e.value << "," << e.weight << "]";
    return out;
  }
};

class Properties
{
public:
  map<string, Enum *> Action;
  map<string, Enum *> UserLevel;
  map<string, Enum *> DeviceClass;
  map<string, Enum *> AccessWay;
  map<string, Enum *> Localization;
  map<string, Enum *> TimeClass;
  map<string, Enum *> Age;
  map<string, Enum *> Group;
  Properties ();
  Properties (map<string, enums::Enum *> a, map<string, enums::Enum *> u,
              map<string, enums::Enum *> d, map<string, enums::Enum *> aw,
              map<string, enums::Enum *> l, map<string, enums::Enum *> t,
              map<string, enums::Enum *> ag, map<string, enums::Enum *> g);
};

}; // namespace enums

#endif
