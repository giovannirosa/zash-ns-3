#ifndef ONTOLOGY
#define ONTOLOGY

#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

#include "../audit/audit.h"
#include "../behavior/configuration.h"
#include "../models/enums_zash.h"
#include "../models/models_zash.h"

struct compareOnt {
    Request key;
    compareOnt(Request r) : key(r) {}

    bool operator()(Ontology *o) {
        return o->userLevel->value == key.user.userLevel->value &&
               o->deviceClass->value == key.device.deviceClass->value;
    }
};

struct compareCap {
    Request key;
    compareCap(Request r) : key(r) {}

    bool operator()(enums::Enum *o) {
        return o->key == key.action.key;
    }
};

class OntologyComponent {
    ConfigurationComponent configurationComponent;
    AuditComponent auditComponent;

   public:
    OntologyComponent() {}
    OntologyComponent(ConfigurationComponent c, AuditComponent a) {
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
    bool verifyOntology(Request req, time_t currenDate) {
        cout << "Ontology Component" << endl;
        cout << "Verify User Level " << req.user.userLevel->key << " with the action " << req.action.key << " on the device class " << req.device.deviceClass->key << endl;

        bool compatible = false;
        auto it = find_if(configurationComponent.ontologies.begin(), configurationComponent.ontologies.end(), compareOnt(req));
        if (it != configurationComponent.ontologies.end()) {
            Ontology *ontology = it[0];
            auto it2 = find_if(ontology->capabilities.begin(), ontology->capabilities.end(), compareCap(req));
            if (it2 != ontology->capabilities.end()) {
                compatible = true;
            }
        }

        string strResult = compatible ? "compatible" : "incompatible";
        cout << "User Level " << req.user.userLevel->key << " is " << strResult << " with the Action " << req.action.key << " on the device class " << req.device.deviceClass->key;
        if (!compatible) {
            auditComponent.ontologyFail.push_back(new AuditEvent(currenDate, req));
        }
        return compatible;
    }
};

#endif