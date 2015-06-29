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
#include "GeoTuple.h"
#include <cstring>

class Environment {
public:
  void eval( const char * str ) { 
    _index = 0;
    _buf = str;
    auto ans = evalOr();
    ans.reduce();
    std::cout << ans << std::endl;
  }
private:
  GeoTuple::Tuples evalOr() {
    auto ts1 = evalAnd();
    while(true) {
      passSpace();
      if ( _buf[_index] == '+' || _buf[_index] == '|' || _buf[_index] == ',' ) {
        ++_index;
        auto ts2 = evalAnd();
        ts1 += ts2;
      }
      else if ( _buf[_index] == '-' ) {
        ++_index;
        auto ts2 = evalAnd();
        ts1 -= ts2;
      }
      else
        break;
    }
    return ts1;
  }
  GeoTuple::Tuples evalAnd() {
    auto ts1 = evalNeg();
    while(true) {
      passSpace();
      if ( _buf[_index] == '*' || _buf[_index] == '&' ) {
        ++_index;
        auto ts2 = evalNeg();
        ts1 *= ts2;
      }
      else
        break;
    }
    return ts1;
  }
  GeoTuple::Tuples evalNeg() {
    auto t1 = evalATuple();
    passSpace();
    if ( _buf[_index] == 't' || _buf[_index] == '\'' ) {
      ++_index;
      return t1.neg();
    }
    else
      return t1;
  }
  GeoTuple::Tuples evalATuple() {
    passSpace();
    if ( _buf[_index] == '(' ) {
      ++_index;
      std::vector<GeoTuple::BYTE> tuple;
      while(true) {
        passSpace();
        int i1 = evalDigit();
        int i2 = evalDigit();
        int i3 = evalDigit();
        tuple.push_back( i1*4 + i2*2 + i3 );
        passSpace();
        if ( _buf[_index] != ',' ) 
          break;
        ++_index;
      }
      if ( _buf[_index] != ')' ) {
        syntaxError();
      }
      else
        ++_index;
      auto ret = GeoTuple::ATuple(tuple.cbegin(), tuple.cend());
      return GeoTuple::Tuples(ret);
    }
    if ( _buf[_index] == '{' ) {
      ++_index;
      auto ts = evalOr();
      passSpace();
      if ( _buf[_index] != '}' ) {
        syntaxError();
      }
      ++_index;
      return ts;
    }
    if ( _buf[_index] != ';' ) {
      syntaxError();
    }
    return GeoTuple::Tuples();
  }

  int evalDigit() {
    char ch = _buf[_index];
    if ( ch == '0' || ch == '1' ) {
      ++_index;
      return ch - '0';
    }
    else {
      return 0;
    }
  }
  void passSpace() {
    while( _buf[_index] == ' ' ) {
      ++_index;
    }
  }
  void forceEnd() {
    throw std::exception();
  }
  void syntaxError() {
    std::cout << "syntax error : [" << &_buf[_index] << "]" << std::endl;
    forceEnd();
  }

private:
  int   _index;
  const char *_buf;
};

int main() {

  Environment environment;
  while(true) {

    std::string line;
    std::getline(std::cin, line);

    try {
      environment.eval( line.c_str() );
    } catch(std::exception &e) {
    }
  }
}

