#ifndef DEVICE
#define DEVICE

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>

using namespace std;

#include "../audit/audit.h"
#include "../decision/authorization.h"
#include "../models/models_zash.h"
#include "data.h"

#define PROOF_EXPIRATION 10  // minutes

class Proof {
   public:
    int user;
    enums::Enum *accessWay;
    time_t date;

    Proof(int u, enums::Enum *a, time_t d) {
        user = u;
        accessWay = a;
        date = d;
    }

    friend ostream &operator<<(ostream &out, Proof const &p) {
        out << "Proof["
            << p.user << ","
            << p.accessWay->key << ",";
        printFormattedTime(p.date, out);
        out << "]";
        return out;
    }
};

struct compareProof {
    Request *key;
    compareProof(Request *r) : key(r) {}

    bool operator()(Proof *p) {
        return (p->user == key->user->id);
    }
};

class DeviceComponent {
   public:
    AuthorizationComponent *authorizationComponent;
    DataComponent *dataComponent;
    AuditComponent *auditComponent;
    vector<Proof *> proofs;
    DeviceComponent(AuthorizationComponent *a, DataComponent *d, AuditComponent *adt) {
        authorizationComponent = a;
        dataComponent = d;
        auditComponent = adt;
    }

    bool explicitAuthentication(Request *req, time_t currentDate) {
        auto it = find_if(proofs.begin(), proofs.end(), compareProof(req));
        if (it == proofs.end()) {
            cout << "Not found proof" << endl;
            int proofInput = req->user->id;
            if (proofInput != req->user->id) {
                auditComponent->invalidProofs.push_back(new AuditEvent(currentDate, req));
                cout << "Proof does not match!" << endl;
            } else {
                auditComponent->validProofs.push_back(new AuditEvent(currentDate, req));
                proofs.push_back(new Proof(req->user->id, req->context->accessWay, currentDate));
            }
        }
        cout << "Proof matches!" << endl;
        return true;
    }

    void clearProofs(time_t currentDate) {
        auto new_end = remove_if(proofs.begin(), proofs.end(), [currentDate](Proof *p) { cout << *p << " | " <<  difftime(currentDate, p->date) << " | " << PROOF_EXPIRATION * 60 << " | " << (difftime(currentDate, p->date) >= PROOF_EXPIRATION * 60) << endl; return difftime(currentDate, p->date) >= PROOF_EXPIRATION * 60; });
        proofs.erase(new_end, proofs.end());
    }

    bool listenRequest(Request *req, time_t currentDate) {
        clearProofs(currentDate);
        dataComponent->updateCurrentState(req);
        bool result = true;
        if (req->device->active) {
            cout << "Active device "
                 << *req->device
                 << " request changing state from "
                 << dataComponent->lastState[req->device->id - 1]
                 << " to "
                 << dataComponent->currentState[req->device->id - 1]
                 << endl;
            ++auditComponent->reqNumber;
            auto fp = bind(&DeviceComponent::explicitAuthentication, this, placeholders::_1, placeholders::_2);
            result = authorizationComponent->authorizeRequest(req, currentDate, fp);
        } else {
            cout << "Passive device "
                 << *req->device
                 << " changed state from "
                 << dataComponent->lastState[req->device->id - 1]
                 << " to "
                 << dataComponent->currentState[req->device->id - 1]
                 << endl;
        }
        dataComponent->updateLastState();
        return result;
    }
};

#endif