#include "zash-authorization.h"

namespace ns3 {

AuthorizationComponent::AuthorizationComponent ()
{
}
AuthorizationComponent::AuthorizationComponent (ConfigurationComponent *c, OntologyComponent *o,
                                                ContextComponent *ctx, ActivityComponent *a,
                                                NotificationComponent *n, AuditComponent *adt)
{
  configurationComponent = c;
  ontologyComponent = o;
  contextComponent = ctx;
  activityComponent = a;
  notificationComponent = n;
  auditComponent = adt;
}

bool
AuthorizationComponent::authorizeRequest (Request *req, time_t currentDate,
                                          function<bool (Request *, time_t)> explicitAuthentication)
{
  *auditComponent->zashOutput << "Authorization Component" << endl;
  *auditComponent->zashOutput << "Processing Request: " << req->id << endl;
  checkUsers (currentDate);
  if (req->user->blocked)
    {
      ++auditComponent->reqDenied;
      *auditComponent->zashOutput << "USER IS BLOCKED - Request is NOT authorized!" << endl;
      return false;
    }
  if (!ontologyComponent->verifyOntology (req, currentDate) ||
      !contextComponent->verifyContext (req, currentDate, explicitAuthentication) ||
      !activityComponent->verifyActivity (req, currentDate, explicitAuthentication))
    {
      req->user->rejected.push_back (currentDate);
      *auditComponent->zashOutput << "User have now " << req->user->rejected.size ()
                                  << " rejected requests!" << endl;
      if (req->user->rejected.size () > (size_t) configurationComponent->blockThreshold)
        {
          auditComponent->blocks.push_back (new AuditEvent (currentDate, req));
          req->user->blocked = true;
          *auditComponent->zashOutput << "User " << req->user->id << " is blocked!" << endl;
          notificationComponent->alertUsers (req->user);
        }
      ++auditComponent->reqDenied;
      *auditComponent->zashOutput << "Request is NOT authorized!" << endl;
      return false;
    }
  ++auditComponent->reqGranted;
  *auditComponent->zashOutput << "Request is authorized!" << endl;
  return true;
}

void
AuthorizationComponent::checkUsers (time_t currentDate)
{
  for (User *user : configurationComponent->users)
    {
      remove_if (user->rejected.begin (), user->rejected.end (), [this, currentDate] (time_t time) {
        return difftime (currentDate, time) < configurationComponent->blockInterval;
      });
    }
}

} // namespace ns3
