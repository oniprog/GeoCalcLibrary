/*
Copyright (c) 2015, Toshiaki Ohkuma
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of GeoCalcLibrary nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
///////////////////////////////////////////////////////////////////////////
// GeoTuple library
///////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <set>
#include <stdexcept>
#include <initializer_list>
#include <bitset>
#include <algorithm>

namespace GeoTuple {

/////////////////////////////////////////////////////////////////////////////
typedef unsigned char BYTE;
/////////////////////////////////////////////////////////////////////////////
struct bad_and_error : public std::logic_error{
    bad_and_error(const std::string &msg) : std::logic_error(msg) {}
};
/////////////////////////////////////////////////////////////////////////////
class Tuples;
/////////////////////////////////////////////////////////////////////////////
class ATuple {
public:
    ATuple() {}
    ATuple(int nSize) : _t(nSize,0) {}

    ATuple(const std::initializer_list<int> &list1 ) : ATuple(list1.begin(), list1.end()) {
    }

    template<typename Iterator>
    ATuple(Iterator begin, Iterator end) {
      std::copy( begin, end, std::back_inserter(_t));
    }

    //
    bool  isZero() const {
      for( int it=0; it<size(); ++it) {
        if ( (_t[it] & 0b111) == 0 )
          return true;
      }
      return false;
    }
    
    // Orができるかを判定する
    friend bool canOr( const ATuple &t1, const ATuple &t2 ) {
        if ( t1.size() != t2.size() )
            return false;
        int nDiff = 0;
        for( int it=0; it<t1.size(); ++it ) {
            if ( t1[it] != t2[it] ) {
                ++nDiff;
                if ( nDiff >= 2 )
                    return false;
            }
        }
        return true;
    }

    // Or処理
    friend ATuple operator+(const ATuple &t1, const ATuple &t2 ) {
      if ( !canOr(t1,t2))
        throw bad_and_error("at (operator+) and error");

      ATuple ans(t1.size());
      for( int it=0; it<t1.size(); ++it ) {
          ans[it] = t1[it] | t2[it];
      }
      return ans;
    }
    void operator +=( const ATuple &t2 ) {
      *this = *this + t2;
    }
    
    // And処理
    friend ATuple operator*(const ATuple &t1, const ATuple &t2 ) {
        if ( t1.size() != t2.size() )
            return ATuple();
        ATuple ans(t1.size());
        for( int it=0; it<t1.size(); ++it) {
            ans[it] = t1[it] & t2[it];
        }
        return ans;
    }
    void operator *=( const ATuple &t2 ) {
        *this = *this * t2;
    }
    BYTE &operator[](int index) {
        return _t[index];
    }
    BYTE operator[](int index) const {
        return _t[index];
    }
    int size() const {
        return static_cast<int>(_t.size());
    }

    //
    friend std::ostream& operator<<( std::ostream &os, const ATuple &t ) {
        os << "(";
        for( int it=0; it<t.size(); ++it ){
            std::bitset<3> tmp(t[it]);
            if (it) os << ",";
            os << tmp;
        }
        return os << ")";
    }

    friend bool operator==(const ATuple &t1, const ATuple &t2 ) {
      if ( t1.size() != t2.size() )
        return false;
      for(int it=0; it<t1.size(); ++it ) {
        if ( t1[it] != t2[it] )
          return false;
      }
      return true;
    }
    friend bool operator!=(const ATuple &t1, const ATuple &t2 ) {
      return !(t1 == t2);
    }

    friend bool operator<(const ATuple &t1, const ATuple &t2 ) {
      if ( t1.size() != t2.size() )
        return t1.size() < t2.size();

      for(int it=0; it<t1.size(); ++it ) {
        if ( t1[it] != t2[it] )
          return t1[it] < t2[it];
      }
      return false;
    }

    Tuples neg() const;

    Tuples operator-(const ATuple &t ) const;

  ATuple simpleOr(const ATuple &t2) const {
    auto ret = *this;
    for(int it=0; it<size(); ++it ) {
      ret[it] |= t2[it];
    }
    return ret;
  }

  int   canSimpleUnion( const ATuple &t2) const {
    auto &t1 = *this;
    int countCanMinus = 0;
    int index = -1, index2 = -1;
    for( int it=0; it<size(); ++it ) {

      if( t1[it] & (7-t2[it] )) {
        ++countCanMinus;
        if ( countCanMinus >= 2 )
          return -1;
        index = it;
      }
      else if ( t1[it] != t2[it] ) {
        index2 = it;
      }
    }
    return index >= 0 ? index : index2;
  }
  bool  simpleUnion( const ATuple &t2, int index ) {

    auto &t1 = *this;
    int previous_number = t1[index];
    t1[index] |= t2[index];
    return previous_number != t1[index];
  }

private:
    std::vector<BYTE> _t;
    friend class Tuples;
};

/////////////////////////////////////////////////////////////////////////////
class Tuples {
public:
  typedef std::set<ATuple> ZATupleSet;
  typedef ZATupleSet::iterator iterator;
  typedef ZATupleSet::const_iterator const_iterator;
public:
  Tuples() {}
  Tuples(const ATuple &t ) {
    _ts.insert(t);
  }
  Tuples removeZero() const {
    Tuples ret;
    if ( _ts.empty() )
      return ret;
    auto ii = cbegin(), ii_end = cend();
    for( ; ii != ii_end; ++ii ) {
      if ( !ii->isZero() ) 
        ret += *ii;
    }
    if ( ret._ts.empty() ) {
      ret += ATuple( _ts.begin()->size() );
    }
    return ret;
  }
  friend Tuples operator+( const Tuples &ts1, const Tuples &ts2 ) {
    Tuples ret(ts1);
    ret += ts2;
    return ret;
  }
  void operator +=(ATuple t1) {
    insertNoCheck( t1 );
  }

  void operator +=( const Tuples &ts ) {
    insertNoCheck(ts);
  }
  friend Tuples operator*( const Tuples &ts, const ATuple &t ) {
    Tuples ans;
    auto ii = ts.cbegin(), ii_end = ts.cend();
    for( ; ii != ii_end; ++ii ) {
      ans += (*ii) * t;
    }
    return ans;
  }
  friend Tuples operator*( const Tuples &ts1, const Tuples &ts2 ) {
    Tuples ans;
    std::for_each( ts2.cbegin(), ts2.cend(), [&ans,&ts1](const ATuple &t) {
      ans += ts1 * t;
    });
    return ans;
  }

  void operator *= ( const Tuples &ts2) {
    *this = (*this) * ts2;
  }

  void operator -=( const ATuple &t2 ) {
    Tuples ans;
    auto ii = cbegin(), ii_end = cend();
    for(; ii != ii_end; ++ii ) {
      auto ts3 = (*ii) - t2;
      ans.insertNoCheck(ts3);
    }
    ans._ts.swap( _ts );
  }

  friend Tuples operator-( const Tuples &ts1, const Tuples &ts2 ) {
    Tuples ans = ts1;
    ans -= ts2;
    return ans;
  }
  friend Tuples operator-( const Tuples &ts1, const ATuple &t2) {
    Tuples ans = ts1;
    ans -= t2;
    return ans;
  }
  void operator -=( const Tuples &ts1 ) {
    auto ii = ts1.cbegin(), ii_end = ts1.cend();
    for( ; ii != ii_end; ++ii ) {
      *this -= *ii;
    }
  }

  Tuples neg() const {
    Tuples ans;
    if ( size() ) {
      auto ii = cbegin(), ii_end = cend();
      ans = ii->neg();
      ++ii;
      for(; ii != ii_end; ++ii ) {
        ans *= ii->neg();
      }
    }
    return ans;
  }

  iterator begin() {
    return _ts.begin();
  }
  iterator end() {
    return _ts.end();
  }
  const_iterator cbegin() const {
    return _ts.cbegin();
  }
  const_iterator cend() const {
    return _ts.cend();
  }
  int size() const {
    return static_cast<int>(_ts.size());
  }

  bool reduceSimpleOr() {
    auto ii = _ts.begin();
    for( ; ii != _ts.end(); ++ii ) {
      auto &t1 = *ii;
      auto jj = ii;
      for(++jj; ii != _ts.end() && jj != _ts.end(); ) {
        auto &t2 = *jj;
        if ( canOr( t1, t2) )  {
          auto t3 = t1.simpleOr(t2);
          jj = _ts.erase(jj);
          ii = _ts.erase(ii);
          _ts.insert(t3);
          return true;
        }
        else 
          ++jj;
      }
    }
    return false;
  }

  bool  reduceSimpleUnion() {
    auto ii = _ts.begin();
    for( ; ii != _ts.end(); ++ii ) {
      auto &t1 = *ii;
      auto jj = _ts.begin();
      for(; jj != _ts.end(); ++jj) {
        if ( ii == jj )
          continue;
        auto &t2 = *jj;
        auto index = t1.canSimpleUnion(t2);
        if ( index >= 0 ) {
          auto t3 = t1;
          if ( t3.simpleUnion(t2, index) ) {
            ii = _ts.erase(ii);
            _ts.insert( t3 );
            return true;
          }
        }
      }
    }
    return false;
  }

  void  reduce() {
    bool loop = true;
    while(loop) {
      loop = false;
      loop |= reduceSimpleOr();
      loop |= reduceSimpleUnion();
      *this = this->removeZero();
    }
  }

  friend std::ostream & operator<< ( std::ostream &os, const Tuples &ts ) {
    bool firstflag = true;
    auto ts2 = ts;
    ts2.reduce();
    std::for_each( ts2.cbegin(), ts2.cend(), [&os,&firstflag](const ATuple &t ) {
      if ( firstflag ) firstflag = false;
      else os << "+";
      os << t;
    });
    return os << std::endl;
  }

  friend bool operator == ( const Tuples &ts1m, const Tuples &ts2m ) {
    auto ts1 = ts1m.removeZero();
    auto ts2 = ts1m.removeZero();
    if ( ts1.size() != ts2.size() )
      return false;
    auto ii1 = ts1.cbegin(), ii_end = ts1.cend();
    auto ii2 = ts2.cbegin();
    for( ; ii1 != ii_end; ++ii1, ++ii2) {
      if ( *ii1 != *ii2 )
        return false;
    }
    return true;
  }
  friend bool operator != ( const Tuples &ts1, const Tuples &ts2 ) {
    return !(ts1 == ts2);
  }

private:
  void insertNoCheck( const ATuple &t ) {
    _ts.insert(t);
  }
  void insertNoCheck( const Tuples &ts ) {
    auto ii = ts.cbegin(), ii_end = ts.cend();
    for(; ii != ii_end; ++ii ) {
      if( !ii->isZero() || size() == 0) {
        _ts.insert(*ii);
      }
    }
  }

private:
  ZATupleSet _ts; 

  friend class ATuple;
};
/////////////////////////////////////////////////////////////////////////////
Tuples ATuple::neg() const {

  Tuples ans;
  for( int it=0; it<size(); ++it ){
    auto tn = *this;
    for(int i1=0; i1<size(); ++i1 ) {
      if ( i1 != it )
        tn[i1] = 0b111;
    }
    tn[it] = 0b111 ^ tn[it];
    if ( !tn.isZero() || ans.size() == 0 )
      ans.insertNoCheck( tn );
  }
  return ans;
}
/////////////////////////////////////////////////////////////////////////////
Tuples ATuple::operator-(const ATuple &t ) const {
  Tuples ans;
  for( int it=0; it<size(); ++it ){
    auto tn = *this;
    tn[it] = t[it] ^ tn[it];
    if ( !tn.isZero() || ans.size() == 0 )
      ans.insertNoCheck( tn );
  }
  return ans;
}

/////////////////////////////////////////////////////////////////////////////
template<typename Iterator>
ATuple InitializeTuple( Iterator begin, Iterator end ) {
    int nSize = std::distance(begin,end);
    ATuple ret( nSize );
    auto ii = begin;
    for(int it=0; it<nSize; ++it ) {
        ret[it] = *ii++;
    }
    return ret;
}

ATuple InitializeTuple( std::initializer_list<BYTE> list1) {
    return InitializeTuple( list1.begin(), list1.end() );
}

Tuples InitializeTuple( std::initializer_list<std::initializer_list<BYTE>> list1 ) {
  Tuples ret;
  std::for_each( list1.begin(), list1.end(), [&ret](std::initializer_list<BYTE> list2) {
    ret += InitializeTuple(list2);
  });
  return ret;
}

/////////////////////////////////////////////////////////////////////////////
// test用
/////////////////////////////////////////////////////////////////////////////
int g_nBadCount = 0;
void AssertEqual( std::initializer_list<BYTE> ansM, const ATuple &result, const char *title) {
  auto ans = InitializeTuple(ansM);
  if ( ans == result ) {
    std::cout << title << " passed." << std::endl;
    return;
  }
  std::cout << title << std::endl;
  std::cout << "correct answer: " << ans;
  std::cout << "result value  : " << result;
  std::cout << std::endl;
  ++g_nBadCount;
}
void AssertEqual( std::initializer_list<std::initializer_list<BYTE>> ansM, const Tuples &result, const char *title) {
  auto ans = InitializeTuple(ansM);
  if ( ans == result ) {
    std::cout << title << " passed." << std::endl;
    return;
  }
  std::cout << title << std::endl;
  std::cout << "correct answer: " << ans;
  std::cout << "result value  : " << result;
  std::cout << std::endl;
  ++g_nBadCount;
}

}; // namespace GeoTuple

