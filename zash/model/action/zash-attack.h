#ifndef ATTACK
#define ATTACK

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <random>

#include "ns3/zash-enums.h"
#include "ns3/zash-models.h"

using namespace std;

namespace ns3 {

class Attack
{
public:
  int id;
  string timeOfAttack;
  bool success;
  string location;
  string accessWay;
  string action;
  int impersonatedUser;
  int device;
  bool scheduled = false;
  bool building;
  Attack (int i, string t, string l, string aw, string a, int iu, int d);

  friend ostream &
  operator<< (ostream &out, Attack const &a)
  {
    out << "Attack[" << a.id << "," << a.timeOfAttack << "," << a.location << "," << a.accessWay
        << "," << a.action << "," << a.impersonatedUser << "," << a.device << "]";
    return out;
  }
};

class AttackManager
{
public:
  vector<Attack *> attacks;
  AttackManager (mt19937 gen, int n, enums::Properties *props, vector<User *> users,
                 vector<Device *> devices, vector<int> dayRange, vector<int> monthRange);

  void printAttacks (stringstream &stream);
};
} // namespace ns3

#endif
