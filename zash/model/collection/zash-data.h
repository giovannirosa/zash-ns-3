#ifndef DATA
#define DATA

using namespace std;

#include "ns3/zash-models.h"
#include "ns3/zash-utils.h"

namespace ns3 {

class DataComponent
{
public:
  vector<int> lastState;
  vector<int> currentState;

  void updateCurrentState (Request *req);

  void updateLastState ();
};
} // namespace ns3

#endif
