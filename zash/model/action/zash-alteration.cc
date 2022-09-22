#include "zash-alteration.h"

namespace ns3 {

Alteration::Alteration (int i, string t, bool a, int d)
{
  id = i;
  timeOfAlteration = t;
  add = a;
  device = d;
}

AlterationManager::AlterationManager (mt19937 gen, int n, vector<Device *> devices,
                                      vector<int> dayRange, vector<int> monthRange)
{
  uniform_int_distribution<mt19937::result_type> distA (0, 1);
  uniform_int_distribution<mt19937::result_type> distD (0, devices.size () - 1);
  uniform_int_distribution<mt19937::result_type> distDay (dayRange[0], dayRange[1]);
  uniform_int_distribution<mt19937::result_type> distMonth (monthRange[0], monthRange[1]);
  uniform_int_distribution<mt19937::result_type> distHour (0, 23);
  uniform_int_distribution<mt19937::result_type> distMinute (0, 59);
  uniform_int_distribution<mt19937::result_type> distSecond (0, 59);
  for (int i = 0; i < n; ++i)
    {
      bool add = distA (gen) == 1;
      int deviceIndex = distD (gen);
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
      Alteration *alteration = new Alteration (i + 1, dateTime, add, deviceIndex);
      alterations.push_back (alteration);
    }
}

void
AlterationManager::printAlterations (AuditComponent *auditModule)
{
  auditModule->fileSim << "Generated alterations are:" << endl;
  for (Alteration *alteration : alterations)
    {
      auditModule->fileSim << *alteration << endl;
    }
  auditModule->fileSim << endl;
}
} // namespace ns3
