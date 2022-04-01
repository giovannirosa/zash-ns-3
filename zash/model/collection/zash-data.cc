#include "zash-data.h"

namespace ns3 {

void DataComponent::updateCurrentState(Request *req) {
  currentState = lastState;
  int currentDeviceState = currentState[req->device->id - 1];
  currentState[req->device->id - 1] = currentDeviceState ? 0 : 1;
}

void DataComponent::updateLastState() { lastState = currentState; }
} // namespace ns3