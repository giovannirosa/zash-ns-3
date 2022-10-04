#include "zash-device.h"

namespace ns3 {

Proof::Proof (int u, enums::Enum *a, time_t d)
{
  user = u;
  accessWay = a;
  date = d;
}

compareProof::compareProof (Request *r) : key (r)
{
}

bool
compareProof::operator() (Proof *p)
{
  return (p->user == key->user->id);
}

compareAttack::compareAttack (int a) : key (a)
{
}

bool
compareAttack::operator() (Attack *a)
{
  return (a->id == key);
}

DeviceComponent::DeviceComponent (AuthorizationComponent *a, DataComponent *d, AuditComponent *adt)
{
  authorizationComponent = a;
  dataComponent = d;
  auditComponent = adt;
}

void
DeviceComponent::processProof (Request *req, bool proof)
{
  if (!proof)
    {
      auditComponent->invalidProofs.push_back (new AuditEvent (req));
      *auditComponent->zashOutput << "Proof does not match!" << endl;
    }
  else
    {
      auditComponent->validProofs.push_back (new AuditEvent (req));
      proofs.push_back (new Proof (req->user->id, req->context->accessWay, req->currentDate));
    }
}

bool
DeviceComponent::explicitAuthentication (Request *req)
{
  auto it = find_if (proofs.begin (), proofs.end (), compareProof (req));
  if (it == proofs.end ())
    {
      *auditComponent->zashOutput << "Not found proof, collecting..." << endl;
      return false;
    }
  *auditComponent->zashOutput << "Proof matches!" << endl;
  return true;
}

void
DeviceComponent::clearProofs (time_t currentDate)
{
  auto new_end = remove_if (proofs.begin (), proofs.end (), [currentDate, this] (Proof *p) {
    *auditComponent->zashOutput << *p << " | " << difftime (currentDate, p->date) << " | "
                                << PROOF_EXPIRATION * 60 << " | "
                                << (difftime (currentDate, p->date) >= PROOF_EXPIRATION * 60)
                                << endl;
    return difftime (currentDate, p->date) >= PROOF_EXPIRATION * 60;
  });
  proofs.erase (new_end, proofs.end ());
}

void
DeviceComponent::processAttack (Request *req, bool result)
{
  auto it = find_if (auditComponent->attackManager->attacks.begin (),
                     auditComponent->attackManager->attacks.end (), compareAttack (req->attackId));
  if (it != auditComponent->attackManager->attacks.end ())
    {
      (*it)->success = result;
      if (!result)
        {
          ++auditComponent->deniedImpersonations;
          auditComponent->fileDenAtt << formatTime (req->currentDate) << " - " << *(*it) << endl;
          if (authorizationComponent->configurationComponent->isBuilding)
            {
              ++auditComponent->deniedAttBuilding;
            }
        }
      else
        {
          auditComponent->fileSucAtt << formatTime (req->currentDate) << " - " << *(*it) << endl;
          if (authorizationComponent->configurationComponent->isBuilding)
            {
              ++auditComponent->successAttBuilding;
            }
        }
    }
}

bool
DeviceComponent::listenRequest (Request *req)
{
  clearProofs (req->currentDate);
  if (req->action->key == "CONTROL")
    {
      dataComponent->updateCurrentState (req);
    }
  bool result = true;
  if (req->action->key == "MANAGE")
    {
      if (req->validated == 0)
        {
          ++auditComponent->reqNumber;
        }
      *auditComponent->zashOutput << "Device " << *req->device << " request manage" << endl;
      auto fp = bind (&DeviceComponent::explicitAuthentication, this, placeholders::_1);
      result = authorizationComponent->authorizeRequest (req, fp);
      if (result && req->validated == 3)
        {
          if (req->device->removed)
            {
              *auditComponent->zashOutput << "Adding Device " << *req->device << " back" << endl;
              authorizationComponent->configurationComponent->addDevice (req->device);
            }
          else
            {
              *auditComponent->zashOutput << "Removing Device " << *req->device << endl;
              authorizationComponent->configurationComponent->remDevice (req->device);
            }
        }
    }
  else if (req->action->key == "VIEW")
    {
      if (req->validated == 0)
        {
          ++auditComponent->reqNumber;
        }
      *auditComponent->zashOutput << "Device " << *req->device << " request view "
                                  << req->device->name << " state" << endl;
      auto fp = bind (&DeviceComponent::explicitAuthentication, this, placeholders::_1);
      result = authorizationComponent->authorizeRequest (req, fp);
    }
  else if (req->device->active)
    {
      if (req->validated == 0)
        {
          ++auditComponent->reqNumber;
        }
      *auditComponent->zashOutput << "Active device " << *req->device
                                  << " request changing state from "
                                  << dataComponent->lastState[req->device->id - 1] << " to "
                                  << dataComponent->currentState[req->device->id - 1] << endl;
      auto fp = bind (&DeviceComponent::explicitAuthentication, this, placeholders::_1);
      result = authorizationComponent->authorizeRequest (req, fp);
    }
  else
    {
      *auditComponent->zashOutput << "Passive device " << *req->device << " changed state from "
                                  << dataComponent->lastState[req->device->id - 1] << " to "
                                  << dataComponent->currentState[req->device->id - 1] << endl;
    }
  dataComponent->updateLastState ();

  if (req->attackId && req->validated == 3)
    {
      processAttack (req, true);
    }

  return result;
}

} // namespace ns3
