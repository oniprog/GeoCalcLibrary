#include "GeoTuple.h"

using namespace GeoTuple;

int main() {

  auto t1 = ATuple({1,7,7});
  auto t2 = ATuple({1,2,3});
  std::cout << (t1-t2) << std::endl;
  std::cout << ((t1-t2)+(t1-t2).neg()) << std::endl;

  return 0;
}
