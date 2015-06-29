//
// 計算ライブラリ
//

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
declare var $:any;

class ATuple3 {
  // 各項を数として保持する配列
  private _t = [];
  constructor(initt=[]) {
    this._t = initt;
  }
  // コピーを作成する
  clone():ATuple3 {
    return new ATuple3(this._t.slice());
  }
  // 長さを返す
  lengtht(): number {
    return this._t.length;
  }
  // すべて111の要素を作成する
  allone(len:number) {
    this._t =[];
    for(var it=0; it<len; ++it)  {
      this._t.push(7);
    }
  }
  // 文字列に変換する
  tos() : string {
    var ret = "";
    for( var it = 0; it<this._t.length; ++it ) {
      if ( it ) ret += ",";
      var v = this._t[it];
      ret += ("" + ~~(v / 4)) + ("" + (~~(v/2)%2)) + ("" + v%2);
    }
    return "(" + ret + ")";
  }
  // 000が含まれているかをチェックする
  isZero():boolean {
    var t = this._t;
    for(var it=0; it<t.length; ++it ) {
      if ( t[it] == 0 )
        return true;
    }
    return false;
  }
  // 100が含まれている数をチェックする
  isNull():boolean {
    var count_face = 0;
    var t = this._t;
    for(var it=0; it<t.length; ++it ) {
      if ( t[it] == 4 ) {
        ++count_face;
        if ( count_face >= 2 )
          return true;
      }
    }
    return false;
  }
  // 
  isFace(face_num:number):boolean {
    var t = this._t;
    return t[face_num] == 4;
  }
  // 内部配列を返す
  toa() : number[] {
    return this._t;
  }
  // ATuple3とのandを計算する
  andt( t2:ATuple3) : ATuple3 {
    return new ATuple3(this.and(t2.toa()));
  }
  // ATuple3とのorを計算する
  ort( t2:ATuple3) : ATuple3[] {
    return this.or(t2.toa());
  }
  // andを計算する
  and(t2:number[]):number[] {
    var t1 = this._t;
    if ( t1.length != t2.length ) 
      throw "syntax error";
    var ret = [];
    for(var it=0; it<t2.length; ++it ) {
      ret[it] = t1[it] & t2[it];
    }
    return ret;
  }
  // orが可能かをチェックする
  canSimpleOr(t2:number[]):boolean {
    var cntDiff = 0;
    var t1 = this._t;
    if ( t1.length != t2.length )
      return false;
    for( var it=0; it<t2.length; ++it ) {
      if ( t1[it] != t2[it] ) {
        ++cntDiff;
        if ( cntDiff >= 2 ) 
          return false;
      }
    }
    return true;
  }
  // unionが可能かどうかをチェックする
  canSimpleUnion(t2:number[]):number{
    var cntNotMinus = 0;
    var t1 = this._t;
    if ( t1.length != t2.length )
      return -1;
    var index = -1;
    var index2 = -1;
    for( var it=0; it<t2.length; ++it ) {
      if ( (t1[it] & (7-t2[it])) > 0 ) {
        ++cntNotMinus;
        if ( cntNotMinus>= 2 ) 
          return -1;
        index = it;
      }
      else if ( t1[it] != t2[it] ) {
        index2 = it;
      }
    }
    return index >= 0 ? index : index2;
  }
  //
  simpleUniont( t2: ATuple3, index:number):boolean{
    return this.simpleUnion(t2.toa(), index );
  }
  simpleUnion( t2:number[] , index:number):boolean{
    var t1 = this._t;
    var previous_value = t1[index];
    t1[index] |= t2[index];
    return t1[index] != previous_value;
  }
  // orを計算する
  or(ta2:number[]):ATuple3[]{
    var ret = [];
    var ta1 = this._t;
    if ( ta1.length != ta2.length ) 
      throw "syntax error";
    var ret = [];
    ret.push( new ATuple3(ta1));
    ret.push( new ATuple3(ta2));
    return ret;
  }
  // 単純なorをする
  orSimple(ta2:number[]):number[] {
    var ret = [];
    if ( !this.canSimpleOr(ta2) ) 
      throw "Internal error";
    var ret = [];
    var ta1 = this._t;
    for( var i=0; i<ta1.length; ++i ) {
      ret[i] = ta1[i] | ta2[i];
    }
    return ret;
  }
  // 単純なorを計算する
  orSimplet( t2: ATuple3):ATuple3 {
    return new ATuple3( this.orSimple(t2.toa()));
  }
  // 補集合を計算する
  neg():ATuple3[] {
    var ret = [];
    var t1 = this._t;
    for(var it=0; it<t1.length; ++it) {
      var t2 = t1.slice();
      for( var i2=0; i2<t1.length; ++i2 ) {
        if ( it == i2 )
          t2[i2] = 7 ^ t2[i2];
        else
          t2[i2] = 7;
      }
      ret.push(new ATuple3(t2));
    }
    return ret;
  }
  // ATuple3に対する引き算を計算する
  minust(t2:ATuple3):ATuple3[] {
    return this.minus(t2.toa());
  }
  // 引き算を計算する
  minus(t2:number[]):ATuple3[] {
    var ret=[];
    var t1 = this._t;
    for(var it=0;it<t1.length; ++it ) {
      var t3 = t1.slice();
      for( var i2=0; i2<t1.length; ++i2 ) {
        if ( it === i2 ) {
          t3[it] = t1[it] & (7^t2[it]);
        }
        else {
          t3[it] = t1[it] & t2[it];
        }
      }
      ret.push( new ATuple3(t3));
    }
    return ret;
  }
  face():ATuple3{
    var ret = [];
    if ( this.isZero() )
      return this.clone();
    var t1 = this._t;
    for( var it=0; it<t1.length; ++it ) {
      ret.push( t1[it] | 4 );
    }
    return new ATuple3(ret);
  }
};

// ATuple3の集合
class Tuples3 {
  private _ts: ATuple3[];
  constructor(ts:ATuple3[]) {
    this._ts = ts;
  }
  // コピー作成する
  clone():Tuples3 {
    return new Tuples3(this._ts.slice());
  }
  length():number {
    return this._ts.length;
  }
  // 文字列に変換する
  tos():string {
    var ts = this._ts;
    var ret = "";
    var firstflag = true;
    for( var it = 0 ; it < ts.length; ++it ) {
      if ( ts[it].isZero() )
        continue;
      if ( firstflag )
        firstflag = false;
      else
        ret += "+";
      ret += ts[it].tos();
    }
    if ( firstflag ) {
      ret += "Φ";
    }
    return ret;
  }
  // 内部配列を返す
  toa():ATuple3[] {
    return this._ts;
  }
  //
  isZero(): boolean {
    var it;
    var ts = this._ts;
    for( it=0; it<ts.length; ++it ) {
      if ( !ts[it].isZero() )
        return false;
    }
    return true;
  }
  // Tuples3を追加する
  pushts(tsm:Tuples3) {
    var tsa2 = tsm.toa();
    for(var it=0; it<tsa2.length; ++it) {
      this.pusht( tsa2[it] );
    }
  }
  // ATuple3を追加する
  pusht(tm:ATuple3) {
    var tsa1 = this.toa();
    tsa1.push( tm );
  }
  // ATuple3とのorを計算する
  ort(tm:ATuple3):Tuples3 {
    var ret = this.clone();
    ret.pusht(tm);
    return ret;
  }
  // Tuples3とのorを計算する
  orts(tsm:Tuples3):Tuples3 {
    var ts1 = this.clone();
    ts1.pushts(tsm);
    return ts1;
  }
  // ATuple3とのandを計算する
  andt( tm:ATuple3): Tuples3 {
    var ret = new Tuples3([]);
    var ts1 = this.toa();
    for( var it=0; it<ts1.length; ++it)  {
      ret.pusht( ts1[it].andt( tm ) );
    }
    return ret;
  }
  // Tuples3とのandを計算する
  andts( tsm:Tuples3):Tuples3 {
    var ret = new Tuples3([]);
    var tsa2 = tsm.toa();
    for( var it=0; it<tsa2.length; ++it)  {
      ret.pushts( this.andt( tsa2[it] ) );
    }
    return ret;
  }
  // ATuple3を引く
  minust(tm:ATuple3):Tuples3 {
    var ts1 = this.clone();
    var tsa1 = ts1.toa();
    var ret = ts1.andts(new Tuples3(tm.neg()));
    return ret;
  }
  // Tuples3を引く
  minusts(tsm:Tuples3):Tuples3 {
    var ts1 = this.clone()
    var tsa2 = tsm.toa();
    for( var it=0; it<tsa2.length; ++it) {
      ts1 = ts1.minust( tsa2[it] );
    }
    return ts1;
  }
  // 
  reduceZero():Tuples3 {
    var tsa1 = this.toa();
    if (tsa1.length <= 1 )
      return;
    var new_tsa = [];
    for(var it=0; it<tsa1.length; ++it) {
      var t1 = tsa1[it];
      if ( t1.isZero() )
        continue;
      new_tsa.push( t1 );
    }
    if (new_tsa.length == 0 ) {
      new_tsa.push( tsa1[0] );
    }
    this._ts = new_tsa;
  }
  // 補集合を計算する
  neg(): Tuples3 {
    var tsa1 = this.toa();
    if ( tsa1.length === 0 )
      return this;
    this.reduceZero();
    var ts2 = new Tuples3(tsa1[0].neg());
    for(var it=1; it<tsa1.length; ++it) {
      ts2 = ts2.andts( new Tuples3(tsa1[it].neg()) );
      ts2.reduce(); // これがないと落ちる
    }
    return ts2;
  }
  // 集合の要素数を返す
  lengtht(): number {
    return this._ts[0].lengtht();
  }
  // 縮約する
  reduce():void {
    var loop = true;
    while(loop) {
      loop = false;
      var ts1 = this.toa();
      for( var i1=0; i1<ts1.length; ++i1 ) {
        for( var i2=i1+1; i2<ts1.length; ++i2 ) {
          if ( ts1[i1].canSimpleOr(ts1[i2].toa())) {
            ts1[i1] = ts1[i1].orSimplet(ts1[i2]);
            ts1.splice( i2, 1 );
            loop = true;
            --i2;
          }
        }
      }
      for( var i1=0; i1<ts1.length; ++i1 ) {
        for( var i2=0; i2<ts1.length; ++i2 ) {
          if ( i1 == i2 )
            continue;
          var index = ts1[i1].canSimpleUnion(ts1[i2].toa());
          if (index >= 0) {
            if ( ts1[i1].simpleUniont( ts1[i2], index ) )
              loop = true;
          }
        }
      }
    }
  }
  face(): Tuples3 {
    var ts1 = this.toa();
    var ret = []
    for( var it=0; it<ts1.length; ++it) {
      ret.push(ts1[it].clone().face());
    }
    return new Tuples3(ret);
  }
  nullfy(): Tuples3 {
    var ts1 = this.toa();
    var ret = []
    for( var it=0; it<ts1.length; ++it) {
      var t1 = ts1[it];
      if ( !t1.isZero() && !t1.isNull() )
        ret.push(ts1[it].clone());
    }
    return new Tuples3(ret);
  }
  selectFace(face_num:number):Tuples3 {
    var ts1 = this.toa();
    var ret = []
    for( var it=0; it<ts1.length; ++it) {
      var t1 = ts1[it];
      if ( t1.isFace(face_num) )
        ret.push(ts1[it].clone());
    }
    return new Tuples3(ret);
  }
};

