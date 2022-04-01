#include "zash-audit.h"

namespace ns3 {

AuditEvent::AuditEvent(time_t t, Request *r) {
  time = t;
  request = r;
}

AuditComponent::AuditComponent() {}

inline void AuditComponent::printEvents(vector<AuditEvent *> events,
                                        char *currDateStr, string type) {
  ofstream outfile(type + "_" + string(currDateStr) + ".txt");
  for (AuditEvent *event : events) {
    outfile << *event << std::endl;
  }
  outfile.close();
}

} // namespace ns3