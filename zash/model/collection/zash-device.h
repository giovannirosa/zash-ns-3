#ifndef DEVICE
#define DEVICE

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>

#include "ns3/packet-sink.h"
#include "ns3/zash-audit.h"
#include "ns3/zash-authorization.h"
#include "ns3/zash-data.h"
#include "ns3/zash-models.h"
#include "ns3/zash-attack.h"

#define PROOF_EXPIRATION 10 // minutes

using namespace std;

namespace ns3 {

class Proof
{
public:
  int user;
  enums::Enum *accessWay;
  time_t date;

  Proof (int u, enums::Enum *a, time_t d);

  friend ostream &
  operator<< (ostream &out, Proof const &p)
  {
    out << "Proof[" << p.user << "," << p.accessWay->key << "," << formatTime (p.date) << "]";
    return out;
  }
};

struct compareProof
{
  Request *key;
  compareProof (Request *r);

  bool operator() (Proof *p);
};

struct compareAttack
{
  int key;
  compareAttack (int i);

  bool operator() (Attack *a);
};

class DeviceComponent
{
public:
  AuthorizationComponent *authorizationComponent;
  DataComponent *dataComponent;
  AuditComponent *auditComponent;
  vector<Proof *> proofs;
  DeviceComponent (AuthorizationComponent *a, DataComponent *d, AuditComponent *adt);

  void processProof (Request *req, bool proof);

  bool explicitAuthentication (Request *req);

  void clearProofs (time_t currentDate);

  void processAttack (Request *req, bool result);

  bool listenRequest (Request *req);
};
} // namespace ns3

#endif
