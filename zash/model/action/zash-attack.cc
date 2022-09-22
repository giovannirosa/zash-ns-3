#include "zash-attack.h"

namespace ns3 {

Attack::Attack (int i, string t, string l, string aw, string a, int iu, int d)
{
  id = i;
  timeOfAttack = t;
  location = l;
  accessWay = aw;
  action = a;
  impersonatedUser = iu;
  device = d;
}

AttackManager::AttackManager (mt19937 gen, int n, enums::Properties *props, vector<User *> users,
                              vector<Device *> devices, vector<int> dayRange,
                              vector<int> monthRange)
{
  discrete_distribution<> distL ({10, 90});
  discrete_distribution<> distAW ({10, 10, 80});
  discrete_distribution<> distA ({10, 40, 50});
  discrete_distribution<> distU ({5, 10, 20, 25, 40});
  uniform_int_distribution<mt19937::result_type> distD (0, devices.size () - 1);
  uniform_int_distribution<mt19937::result_type> distDay (dayRange[0], dayRange[1]);
  uniform_int_distribution<mt19937::result_type> distMonth (monthRange[0], monthRange[1]);
  uniform_int_distribution<mt19937::result_type> distHour (0, 23);
  uniform_int_distribution<mt19937::result_type> distMinute (0, 59);
  uniform_int_distribution<mt19937::result_type> distSecond (0, 59);
  for (int i = 0; i < n; ++i)
    {
      int locationIndex = distL (gen);
      int accessWayIndex = distAW (gen);
      int actionIndex = distA (gen);
      int userIndex = distU (gen);
      int deviceIndex = distD (gen);
      while (!devices[deviceIndex]->active && props->actions[actionIndex] == "CONTROL")
        {
          deviceIndex = distD (gen);
        }
      string day = to_string (distDay (gen));
      string month = to_string (distMonth (gen));
      string hour = to_string (distHour (gen));
      string minute = to_string (distMinute (gen));
      string second = to_string (distSecond (gen));
      string dateTime = "2022-" + string (2 - month.size (), '0').append (month) + "-" +
                        string (2 - day.size (), '0').append (day) + " " +
                        string (2 - hour.size (), '0').append (hour) + ":" +
                        string (2 - minute.size (), '0').append (minute) + ":" +
                        string (2 - second.size (), '0').append (second);
      Attack *attack = new Attack (i + 1, dateTime, props->localizations[locationIndex],
                                   props->accessWays[accessWayIndex], props->actions[actionIndex],
                                   users[userIndex]->id, deviceIndex);
      attacks.push_back (attack);
    }
}

void
AttackManager::printAttacks (AuditComponent *auditModule)
{
  auditModule->fileSim << "Generated attacks are:" << endl;
  for (Attack *attack : attacks)
    {
      auditModule->fileSim << *attack << endl;
    }
  auditModule->fileSim << endl;
}
} // namespace ns3
