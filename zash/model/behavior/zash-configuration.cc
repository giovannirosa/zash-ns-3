#include "zash-configuration.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ConfigurationComponent");

ConfigurationComponent::ConfigurationComponent() {}
ConfigurationComponent::ConfigurationComponent(int bt, int bi, int bdi,
                                               vector<Device *> d,
                                               vector<User *> u,
                                               vector<Ontology *> o,
                                               AuditComponent *a) {
  blockThreshold = bt;
  blockInterval = bi;
  buildInterval = bdi;
  devices = d;
  users = u;
  ontologies = o;
  auditModule = a;
}

bool ConfigurationComponent::addDevice(Device *d) {
  if (find(devices.begin(), devices.end(), d) != devices.end()) {
    NS_LOG_INFO("Cannot add " << d->name << " because it already exists!");
    return false;
  }

  auto it = find_if(devices.begin(), devices.end(),
                    [&d](Device *di) -> bool { return d->id == di->id; });
  if (it != devices.end()) {
    NS_LOG_INFO("Cannot add " << d->name << " because it has an existing ID!");
    return false;
  }

  devices.push_back(d);

  if (devices.size() > auditModule->maxDeviceNumber) {
    auditModule->maxDeviceNumber = devices.size();
    auditModule->deviceExtensibility =
        auditModule->maxDeviceNumber - auditModule->minDeviceNumber;
  }

  return true;
}

bool ConfigurationComponent::remDevice(Device *d) {
  devices.erase(remove_if(devices.begin(), devices.end(),
                          [&d](Device *di) { return di && (di == d); }));
  if (devices.size() < auditModule->minDeviceNumber) {
    auditModule->minDeviceNumber = devices.size();
    auditModule->deviceExtensibility =
        auditModule->maxDeviceNumber - auditModule->minDeviceNumber;
  }
  return true;
}

} // namespace ns3