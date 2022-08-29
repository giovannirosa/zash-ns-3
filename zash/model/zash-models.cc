#include "zash-models.h"

namespace ns3 {

Context::Context ()
{
}
Context::Context (enums::Enum *aw, enums::Enum *loc, enums::Enum *g)
{
  accessWay = aw;
  localization = loc;
  group = g;
}

Ontology::Ontology (enums::Enum *ul, enums::Enum *dc, vector<enums::Enum *> c)
{
  userLevel = ul;
  deviceClass = dc;
  capabilities = c;
}

User::User ()
{
}
User::User (int i, enums::Enum *ul, enums::Enum *a)
{
  id = i;
  userLevel = ul;
  age = a;
  rejected = {};
  startInterval = (time_t) (-1);
  blocked = false;
}

Device::Device ()
{
}
Device::Device (int i, string n, enums::Enum *dc, int r, bool a)
{
  id = i;
  name = n;
  deviceClass = dc;
  room = r;
  active = a;
}
Device::Device (int i, string n, enums::Enum *dc, int r, bool a, int apv, int p)
{
  id = i;
  name = n;
  deviceClass = dc;
  room = r;
  active = a;
  ap = apv;
  pos = p;
}

Request::Request ()
{
}
Request::Request (int i, Device *d, User *u, Context *c, enums::Enum *a)
{
  id = i;
  device = d;
  user = u;
  context = c;
  action = a;
}

} // namespace ns3
