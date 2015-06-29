#include "GeoTuple.h"

// 補助関数
// 簡単にATupleを作れるように
inline
GeoTuple::ATuple T(std::initializer_list<GeoTuple::BYTE> list1) {
  return GeoTuple::InitializeTuple(list1);
}

// 簡単にTuplesを作れるように
inline
GeoTuple::Tuples T(std::initializer_list<std::initializer_list<GeoTuple::BYTE>> list1) {
  return GeoTuple::InitializeTuple(list1);
}

// ATuple -> Tuplesへの変換
inline
GeoTuple::Tuples G(const GeoTuple::ATuple &t) {
  return GeoTuple::Tuples(t);
}

/////////////////////////////////////////////////////////////////////////////
int main() {

    auto t1 = T({ 0b111, 0b110 });
    auto t2 = T({ 0b100, 0b101 });
    auto t3 = T({ 0b100, 0b010 });

    std::cout<< "===> test begin" << std::endl;
    GeoTuple::AssertEqual( {{0b100, 0b101}, {0b111, 0b110}}, G(t1) + t2, "or1");
    GeoTuple::AssertEqual( {{0b100, 0b111}}, G(t2) + t3, "or2");
    GeoTuple::AssertEqual( {{0b100, 0b111}, {0b111, 0b110}}, (G(t1) + t2) + (G(t2) + t3), "or3");

    GeoTuple::AssertEqual( {0b100, 0b100}, t1 * t2, "and1");
    GeoTuple::AssertEqual( {0b100, 0b000}, t2 * t3, "and2");

    GeoTuple::AssertEqual( {{0b111, 0b001}}, t1.neg(), "neg1");
    GeoTuple::AssertEqual( {{0b011, 0b101}, {0b111, 0b010}}, t2.neg(), "neg2");
    GeoTuple::AssertEqual( {{0b111, 0b001}, {0b011, 0b101}, {0b111, 0b010}}, (G(t1)+t2).neg(), "neg3");

    GeoTuple::AssertEqual( {}, G(t1) * t1.neg(), "neg and1");
    GeoTuple::AssertEqual( {{0b111,0b111}}, G(t1) + t1.neg(), "neg or1");
    GeoTuple::AssertEqual( {}, (G(t1)+t2) * (G(t1)+t2).neg(), "neg and2");
    GeoTuple::AssertEqual( {{0b111, 0b111}}, (G(t1)+t2) + (G(t1)+t2).neg(), "neg or2");

    auto t4 = T({0b001, 0b011});
    auto t5 = T({0b011, 0b001});

    GeoTuple::AssertEqual( {{0b001, 0b011}, {0b011, 0b001}}, G(t4)+t5, "or4");

    if ( GeoTuple::g_nBadCount == 0) {
      std::cout << "===> all test ok" << std::endl;
    }
}

