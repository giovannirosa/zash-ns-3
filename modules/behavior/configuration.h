#ifndef CONFIGURATION
#define CONFIGURATION

using namespace std;

#include "../models/models_zash.h"

class ConfigurationComponent {
   public:
    int blockThreshold;
    int blockInterval;
    int buildInterval;
    vector<Device *> devices;
    vector<User *> users;
    vector<Ontology *> ontologies;
    ConfigurationComponent() {}
    ConfigurationComponent(int bt, int bi, int bdi, vector<Device *> d, vector<User *> u, vector<Ontology *> o) {
        blockThreshold = bt;
        blockInterval = bi;
        buildInterval = bdi;
        devices = d;
        users = u;
        ontologies = o;
    }
};

#endif