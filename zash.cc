#include <iostream>
#include <vector>

using namespace std;

#include "audit/audit.h"
#include "activity.h"
#include "authorization.h"
#include "configuration.h"
#include "context.h"
#include "data.h"
#include "device.h"
#include "enums_zash.h"
#include "models_zash.h"
#include "notification.h"
#include "ontology.h"
#include "csv_reader.h"

#define NUMBER_OF_DEVICES 29
#define DATE_COL 30

int main() {
    cout << __cplusplus << endl;

    vector<User *> users = {
        new User(1, enums::UserLevel.at("ADMIN"), enums::Age.at("ADULT")),
        new User(2, enums::UserLevel.at("ADULT"), enums::Age.at("ADULT")),
        new User(3, enums::UserLevel.at("CHILD"), enums::Age.at("TEEN")),
        new User(4, enums::UserLevel.at("CHILD"), enums::Age.at("KID")),
        new User(5, enums::UserLevel.at("VISITOR"), enums::Age.at("ADULT"))};

    vector<Device *> devices = {
        new Device(1, "Wardrobe", enums::DeviceClass.at("NONCRITICAL"), enums::BEDROOM, true),            // wardrobe
        new Device(2, "TV", enums::DeviceClass.at("NONCRITICAL"), enums::LIVINGROOM, true),               // tv
        new Device(3, "Oven", enums::DeviceClass.at("CRITICAL"), enums::KITCHEN, true),                   // oven
        new Device(4, "Office Light", enums::DeviceClass.at("NONCRITICAL"), enums::OFFICE, true),         // officeLight
        new Device(5, "Office Door Lock", enums::DeviceClass.at("CRITICAL"), enums::OFFICE, true),        // officeDoorLock
        new Device(6, "Office Door", enums::DeviceClass.at("NONCRITICAL"), enums::OFFICE, true),          // officeDoor
        new Device(7, "Office Carpet", enums::DeviceClass.at("NONCRITICAL"), enums::OFFICE, false),       // officeCarp
        new Device(8, "Office", enums::DeviceClass.at("NONCRITICAL"), enums::OFFICE, false),              // office
        new Device(9, "Main Door Lock", enums::DeviceClass.at("CRITICAL"), enums::HOUSE, true),           // mainDoorLock
        new Device(10, "Main Door", enums::DeviceClass.at("NONCRITICAL"), enums::HOUSE, true),            // mainDoor
        new Device(11, "Living Light", enums::DeviceClass.at("NONCRITICAL"), enums::LIVINGROOM, true),    // livingLight
        new Device(12, "Living Carpet", enums::DeviceClass.at("NONCRITICAL"), enums::LIVINGROOM, false),  // livingCarp
        new Device(13, "Kitchen Light", enums::DeviceClass.at("NONCRITICAL"), enums::KITCHEN, true),      // kitchenLight
        new Device(14, "Kitchen Door Lock", enums::DeviceClass.at("CRITICAL"), enums::KITCHEN, true),     // kitchenDoorLock
        new Device(15, "Kitchen Door", enums::DeviceClass.at("NONCRITICAL"), enums::KITCHEN, true),       // kitchenDoor
        new Device(16, "Kitchen Carpet", enums::DeviceClass.at("NONCRITICAL"), enums::KITCHEN, false),    // kitchenCarp
        new Device(17, "Hallway Light", enums::DeviceClass.at("NONCRITICAL"), enums::HOUSE, true),        // hallwayLight
        new Device(18, "Fridge", enums::DeviceClass.at("CRITICAL"), enums::KITCHEN, true),                // fridge
        new Device(19, "Couch", enums::DeviceClass.at("NONCRITICAL"), enums::LIVINGROOM, false),          // couch
        new Device(20, "Bedroom Light", enums::DeviceClass.at("NONCRITICAL"), enums::BEDROOM, true),      // bedroomLight
        new Device(21, "Bedroom Door Lock", enums::DeviceClass.at("CRITICAL"), enums::BEDROOM, true),     // bedroomDoorLock
        new Device(22, "Bedroom Door", enums::DeviceClass.at("NONCRITICAL"), enums::BEDROOM, true),       // bedroomDoor
        new Device(23, "Bedroom Carpet", enums::DeviceClass.at("NONCRITICAL"), enums::BEDROOM, false),    // bedroomCarp
        new Device(24, "Bed Table Lamp", enums::DeviceClass.at("NONCRITICAL"), enums::BEDROOM, true),     // bedTableLamp
        new Device(25, "Bed", enums::DeviceClass.at("NONCRITICAL"), enums::BEDROOM, false),               // bed
        new Device(26, "Bathroom Light", enums::DeviceClass.at("NONCRITICAL"), enums::BATHROOM, true),    // bathroomLight
        new Device(27, "Bathroom Door Lock", enums::DeviceClass.at("CRITICAL"), enums::BATHROOM, true),   // bathroomDoorLock
        new Device(28, "Bathroom Door", enums::DeviceClass.at("NONCRITICAL"), enums::BATHROOM, true),     // bathroomDoor
        new Device(29, "Bathroom Carpet", enums::DeviceClass.at("NONCRITICAL"), enums::BATHROOM, false)   // bathroomCarp
    };

    vector<enums::Enum *> visitorCriticalCap = {};
    Ontology *visitorCritical = new Ontology(enums::UserLevel.at("VISITOR"), enums::DeviceClass.at("CRITICAL"), visitorCriticalCap);

    vector<enums::Enum *> childCriticalCap = {enums::Action.at("VIEW")};
    childCriticalCap.insert(childCriticalCap.end(), visitorCriticalCap.begin(), visitorCriticalCap.end());
    Ontology *childCritical = new Ontology(enums::UserLevel.at("CHILD"), enums::DeviceClass.at("CRITICAL"), childCriticalCap);

    vector<enums::Enum *> adultCriticalCap = {enums::Action.at("CONTROL")};
    adultCriticalCap.insert(adultCriticalCap.end(), childCriticalCap.begin(), childCriticalCap.end());
    Ontology *adultCritical = new Ontology(enums::UserLevel.at("ADULT"), enums::DeviceClass.at("CRITICAL"), adultCriticalCap);

    vector<enums::Enum *> adminCriticalCap = {enums::Action.at("MANAGE")};
    adminCriticalCap.insert(adminCriticalCap.end(), adultCriticalCap.begin(), adultCriticalCap.end());
    Ontology *adminCritical = new Ontology(enums::UserLevel.at("ADMIN"), enums::DeviceClass.at("CRITICAL"), adminCriticalCap);

    vector<enums::Enum *> visitorNonCriticalCap = {enums::Action.at("VIEW"), enums::Action.at("CONTROL")};
    Ontology *visitorNonCritical = new Ontology(enums::UserLevel.at("VISITOR"), enums::DeviceClass.at("NONCRITICAL"), visitorNonCriticalCap);

    vector<enums::Enum *> childNonCriticalCap = {};
    childNonCriticalCap.insert(childNonCriticalCap.end(), visitorNonCriticalCap.begin(), visitorNonCriticalCap.end());
    Ontology *childNonCritical = new Ontology(enums::UserLevel.at("CHILD"), enums::DeviceClass.at("NONCRITICAL"), childNonCriticalCap);

    vector<enums::Enum *> adultNonCriticalCap = {enums::Action.at("MANAGE")};
    adultNonCriticalCap.insert(adultNonCriticalCap.end(), childNonCriticalCap.begin(), childNonCriticalCap.end());
    Ontology *adultNonCritical = new Ontology(enums::UserLevel.at("ADULT"), enums::DeviceClass.at("NONCRITICAL"), adultNonCriticalCap);

    vector<enums::Enum *> adminNonCriticalCap = {};
    adminNonCriticalCap.insert(adminNonCriticalCap.end(), adultNonCriticalCap.begin(), adultNonCriticalCap.end());
    Ontology *adminNonCritical = new Ontology(enums::UserLevel.at("ADMIN"), enums::DeviceClass.at("NONCRITICAL"), adminNonCriticalCap);

    vector<Ontology *> ontologies = {visitorCritical,
                                     childCritical,
                                     adultCritical,
                                     adminCritical,
                                     visitorNonCritical,
                                     childNonCritical,
                                     adultNonCritical,
                                     adminNonCritical};

    // Audit Module
    AuditComponent auditModule;

    // Behavior Module
    ConfigurationComponent configurationComponent = ConfigurationComponent(3, 24, 32, devices, users, ontologies);
    NotificationComponent notificationComponent = NotificationComponent(configurationComponent);

    // Collection Module
    DataComponent dataComponent;

    // Decision Module
    OntologyComponent ontologyComponent = OntologyComponent(configurationComponent, auditModule);
    ContextComponent contextComponent = ContextComponent(configurationComponent, auditModule);
    ActivityComponent activityComponent = ActivityComponent(dataComponent, configurationComponent, auditModule);
    AuthorizationComponent authorizationComponent = AuthorizationComponent(configurationComponent, ontologyComponent, contextComponent, activityComponent, notificationComponent, auditModule);

    // Collection Module
    DeviceComponent deviceComponent = DeviceComponent(authorizationComponent, dataComponent, auditModule);

    User *simUser = users[2];
    Context simContext = Context(enums::AccessWay.at("PERSONAL"), enums::Localization.at("INTERNAL"), enums::Group.at("ALONE"));
    enums::Enum *simAction = enums::Action.at("CONTROL");

    int idReq = 0;

    ifstream file("data/d6_2m_0tm.csv");

    for (auto& row : CSVRange(file)) {
        cout << row[DATE_COL] << std::endl;
        time_t currentDate = strToTime(string(row[DATE_COL]));
        
        vector<int> currentState = rowToState(row);
    }
    

    return 0;
}