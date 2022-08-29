#include "zash-enums.h"

namespace enums {

Enum::Enum ()
{
}
Enum::Enum (string k, int v, int w)
{
  key = k;
  value = v;
  weight = w;
}

Properties::Properties ()
{
}
Properties::Properties (map<string, enums::Enum *> a, map<string, enums::Enum *> u,
                        map<string, enums::Enum *> d, map<string, enums::Enum *> aw,
                        map<string, enums::Enum *> l, map<string, enums::Enum *> t,
                        map<string, enums::Enum *> ag, map<string, enums::Enum *> g)
{
  Action = a;
  UserLevel = u;
  DeviceClass = d;
  AccessWay = aw;
  Localization = l;
  TimeClass = t;
  Age = ag;
  Group = g;
  // Action = {{"MANAGE", new enums::Enum ("MANAGE", 1, 40)},
  //           {"CONTROL", new enums::Enum ("CONTROL", 2, 20)},
  //           {"VIEW", new enums::Enum ("VIEW", 3, 0)}};
  // UserLevel = {{"ADMIN", new enums::Enum ("ADMIN", 1, 70)},
  //              {"ADULT", new enums::Enum ("ADULT", 2, 50)},
  //              {"CHILD", new enums::Enum ("CHILD", 3, 30)},
  //              {"VISITOR", new enums::Enum ("VISITOR", 4, 0)}};
  // DeviceClass = {{"CRITICAL", new enums::Enum ("CRITICAL", 1, 30)},
  //                {"NONCRITICAL", new enums::Enum ("NONCRITICAL", 2, 0)}};
  // AccessWay = {{"REQUESTED", new enums::Enum ("REQUESTED", 1, 30)},
  //              {"HOUSE", new enums::Enum ("HOUSE", 2, 20)},
  //              {"PERSONAL", new enums::Enum ("PERSONAL", 3, 10)}};
  // Localization = {{"INTERNAL", new enums::Enum ("INTERNAL", 1, 30)},
  //                 {"EXTERNAL", new enums::Enum ("EXTERNAL", 2, 10)}};
  // TimeClass = {{"COMMON", new enums::Enum ("COMMON", 1, 20)},
  //              {"UNCOMMON", new enums::Enum ("UNCOMMON", 2, 10)}};
  // Age = {{"ADULT", new enums::Enum ("ADULT", 1, 30)},
  //        {"TEEN", new enums::Enum ("TEEN", 2, 20)},
  //        {"KID", new enums::Enum ("KID", 3, 10)}};
  // Group = {{"TOGETHER", new enums::Enum ("TOGETHER", 1, 10)},
  //          {"ALONE", new enums::Enum ("ALONE", 2, 0)}};
}

} // namespace enums
