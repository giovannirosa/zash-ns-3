#include "zash-activity.h"

namespace ns3 {

ActivityComponent::ActivityComponent ()
{
}
ActivityComponent::ActivityComponent (DataComponent *d, ConfigurationComponent *c,
                                      AuditComponent *a)
{
  dataComponent = d;
  configurationComponent = c;
  auditComponent = a;
}

bool
ActivityComponent::verifyActivity (Request *req, time_t currentDate,
                                   function<bool (Request *, time_t)> explicitAuthentication)
{
  *auditComponent->zashOutput << "Activity Component" << endl;
  vector<int> lastState = dataComponent->lastState;
  vector<int> currentState = dataComponent->currentState;
  if (lastState.size () != currentState.size ())
    {
      resetMarkov ();
    }
  checkBuilding (currentDate);
  if (!isMarkovBuilding)
    {
      *auditComponent->zashOutput << "Verify activities" << endl;
      *auditComponent->zashOutput << "From: " << vecToStr (lastState) << endl;
      *auditComponent->zashOutput << "To: " << vecToStr (currentState) << endl;
      float prob = markovChain->getProbability (currentState, lastState);
      *auditComponent->zashOutput << "Probability = " << prob << endl;
      if (prob < configurationComponent->markovThreshold)
        {
          auditComponent->activityFail.push_back (new AuditEvent (currentDate, req));
          *auditComponent->zashOutput << "Activity is NOT valid! Requires proof of identity!"
                                      << endl;
          if (!explicitAuthentication (req, currentDate))
            {
              return false;
            }
        }
      *auditComponent->zashOutput << "Activity is valid!" << endl;
    }
  else
    {
      *auditComponent->zashOutput << "Markov Chain is still building for " << req->device->name
                                  << endl;
    }
  markovChain->buildTransition (currentState, lastState);
  return true;
}

// check if markov build time expired
void
ActivityComponent::checkBuilding (time_t currentDate)
{
  if (difftime (limitDate, (time_t) (-1)) == 0)
    {
      limitDate = currentDate + configurationComponent->buildInterval * 24 * 60 * 60; // add days
    }
  else if (isMarkovBuilding && difftime (currentDate, limitDate) > 0)
    {
      isMarkovBuilding = false;
      *auditComponent->zashOutput << "Markov Chain completed building transition matrix at "
                                  << formatTime (currentDate) << endl;
    }
}

void
ActivityComponent::resetMarkov ()
{
  delete markovChain;
  markovChain = new MarkovChain ();
  isMarkovBuilding = true;
  limitDate = (time_t) (-1);
}

} // namespace ns3
