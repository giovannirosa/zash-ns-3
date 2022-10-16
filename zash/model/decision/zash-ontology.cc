#include "zash-ontology.h"

namespace ns3 {

compareOnt::compareOnt (Request *r) : key (r)
{
}

bool
compareOnt::operator() (Ontology *o)
{
  return o->userLevel->value == key->user->userLevel->value &&
         o->deviceClass->value == key->device->deviceClass->value;
}

compareCap::compareCap (Request *r) : key (r)
{
}

bool
compareCap::operator() (enums::Enum *o)
{
  return o->key == key->action->key;
}

OntologyComponent::OntologyComponent ()
{
}
OntologyComponent::OntologyComponent (ConfigurationComponent *c, AuditComponent *a)
{
  configurationComponent = c;
  auditComponent = a;
}

// common ontologies like:
//   - critical devices:
//       - visitor cannot even visualize
//       - kids can only visualize
//       - adults can only visualize and control
//       - admins can visualize, control and manage
//   - non-critical devices:
//       - visitor and kids can visualize and control
//       - adults and admins can visualize, control and manage
bool
OntologyComponent::verifyOntology (Request *req)
{
  ++req->validated;
  // cout << "Ontology Component" << endl;
  *auditComponent->zashOutput << "Ontology Component" << endl;
  *auditComponent->zashOutput << "Verify User Level " << req->user->userLevel->key
                              << " with the action " << req->action->key << " on the device class "
                              << req->device->deviceClass->key << endl;

  bool compatible = false;
  auto it = find_if (configurationComponent->ontologies.begin (),
                     configurationComponent->ontologies.end (), compareOnt (req));
  if (it != configurationComponent->ontologies.end ())
    {
      Ontology *ontology = it[0];
      auto it2 = find_if (ontology->capabilities.begin (), ontology->capabilities.end (),
                          compareCap (req));
      if (it2 != ontology->capabilities.end ())
        {
          compatible = true;
        }
    }

  string strResult = compatible ? "compatible" : "incompatible";
  *auditComponent->zashOutput << "User Level " << req->user->userLevel->key << " is " << strResult
                              << " with the Action " << req->action->key << " on the device class "
                              << req->device->deviceClass->key << endl;
  if (!compatible)
    {
      auditComponent->ontologyFail.push_back (new AuditEvent (req));
    }
  return compatible;
}
} // namespace ns3
