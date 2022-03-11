#ifndef AUDIT
#define AUDIT

#include <ctime>
#include <iostream>

using namespace std;

#include "../models/models_zash.h"

class AuditEvent {
   public:
    time_t time;
    Request *request;
    AuditEvent(time_t t, Request *r) {
        time = t;
        request = r;
    }

    friend ostream &operator<<(ostream &out, AuditEvent const &a) {
        printFormattedTime(a.time, out);
        out << " - " << *a.request << endl;
        return out;
    }
};

class AuditComponent {
   public:
    vector<AuditEvent *> ontologyFail;
    vector<AuditEvent *> contextFail;
    vector<AuditEvent *> activityFail;
    vector<AuditEvent *> blocks;
    vector<AuditEvent *> attacks;
    vector<AuditEvent *> validProofs;
    vector<AuditEvent *> invalidProofs;
    int reqNumber = 0;
    int reqGranted = 0;
    int reqDenied = 0;
    AuditComponent() {}

    void printEvents(vector<AuditEvent *> events, char *currDateStr, string type) {
        ofstream outfile(type + "_" + string(currDateStr) + ".txt");
        for (AuditEvent *event : events) {
            outfile << *event << std::endl;
        }
        outfile.close();
    }
};

#endif