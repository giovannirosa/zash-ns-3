#ifndef ALTERATION
#define ALTERATION

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <random>
#include <algorithm>

#include "ns3/zash-enums.h"
#include "ns3/zash-models.h"

using namespace std;

namespace ns3 {

class Alteration
{
public:
  int id;
  string timeOfAlteration;
  bool scheduled = false;
  int device;
  Alteration (int i, string t, int d);

  friend ostream &
  operator<< (ostream &out, Alteration const &a)
  {
    out << "Alteration[" << a.id << "," << a.timeOfAlteration << "," << a.device << "]";
    return out;
  }
};

class AlterationManager
{
public:
  vector<Alteration *> alterations;
  AlterationManager (mt19937 gen, int n, vector<Device *> devices, vector<int> dayRange,
                     vector<int> monthRange, vector<string> datesList);

  void printAlterations (stringstream &stream);
};
} // namespace ns3

#endif
