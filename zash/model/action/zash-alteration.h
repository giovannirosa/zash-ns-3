#ifndef ALTERATION
#define ALTERATION

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <random>

#include "ns3/zash-enums.h"
#include "ns3/zash-audit.h"
#include "ns3/zash-models.h"

using namespace std;

namespace ns3 {

class Alteration
{
public:
  int id;
  string timeOfAlteration;
  bool add;
  int device;
  Alteration (int i, string t, bool a, int d);

  friend ostream &
  operator<< (ostream &out, Alteration const &a)
  {
    out << "Alteration[" << a.id << "," << a.timeOfAlteration << "," << a.add << "," << a.device
        << "]";
    return out;
  }
};

class AlterationManager
{
public:
  vector<Alteration *> alterations;
  AlterationManager (mt19937 gen, int n, vector<Device *> devices, vector<int> dayRange,
                     vector<int> monthRange);

  void printAlterations (AuditComponent *auditModule);
};
} // namespace ns3

#endif
