#include "zash-enums.h"

namespace enums {

Enum::Enum() {}
Enum::Enum(string k, int v, int w) {
  key = k;
  value = v;
  weight = w;
}

} // namespace enums