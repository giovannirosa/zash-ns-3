#ifndef ONTOLOGY
#define ONTOLOGY

#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

#include "ns3/zash-audit.h"
#include "ns3/zash-configuration.h"
#include "ns3/zash-enums.h"
#include "ns3/zash-models.h"

namespace ns3 {

struct compareOnt {
  Request *key;
  compareOnt(Request *r);

  bool operator()(Ontology *o);
};

struct compareCap {
  Request *key;
  compareCap(Request *r);

  bool operator()(enums::Enum *o);
};

class OntologyComponent {
  ConfigurationComponent *configurationComponent;
  AuditComponent *auditComponent;

public:
  OntologyComponent();
  OntologyComponent(ConfigurationComponent *c, AuditComponent *a);

  // common ontologies like:
  //   - critical devices:
  //       - visitor cannot even visualize
  //       - kids can only visualize
  //       - adults can only visualize and control
  //       - admins can visualize, control and manage
  //   - non-critical devices:
  //       - visitor and kids can visualize and control
  //       - adults and admins can visualize, control and manage
  bool verifyOntology(Request *req, time_t currenDate);
};
} // namespace ns3

#endif