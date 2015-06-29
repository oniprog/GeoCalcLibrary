#Copyright (c) 2015, Toshiaki Ohkuma
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#* Redistributions of source code must retain the above copyright notice, this
#  list of conditions and the following disclaimer.
#
#* Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation
#  and/or other materials provided with the distribution.
#
#* Neither the name of GeoCalcLibrary nor the names of its
#  contributors may be used to endorse or promote products derived from
#  this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

class P 
  def initialize(elist) 
    @p1 = []
    elist.each{ |e| 
      if e.is_a? String then
        @p1.push e.to_i(2) 
      else
        @p1.push e
      end
    }
  end
  def clone
    return P.new( @p1 )
  end
  def zero?
    return @p1.any?{|e| e == 0}
  end
  def merge?(p2)
    return false if p2 == nil
    return false if @p1.length != p2.length
    count_difference = 0
    @p1.length.times{ |i| count_difference += 1 if @p1[i] != p2[i] }
    return count_difference <= 1
  end
  def merge_or(p2)
    result = []
    @p1.length.times{ |i| result.push(@p1[i] | p2[i]) }
    return P.new(result)
  end
  def and(p2) 
    return if @p1.length != p2.length
    result = []
    @p1.length.times {|i| result[i] = @p1[i] & p2[i]}
    return P.new(result)
  end
  def neg
    result = []
    @p1.length.times{ |j|
      p3 = []
      @p1.length.times{ |i| 
        if i == j then     
          p3.push(7-@p1[i]) 
        else
          p3.push(7) 
        end
      }
      result.push(P.new(p3))
    }
    return PS.new(result)
  end
  def minus(p2)
    return if @p1.length != p2.length
    result = []
    @p1.length.times{ |i| 
      p3 = []
      @p1.length.times{ |j|
        if i == j then     
          p3.push(@p1[i] & (7-p2[i])) 
        else
          p3.push(@p1[i]) 
        end
      }
      p3 = P.new(p3)
      result.push(p3) if not p3.zero?
    }
    return PS.new(result)
  end
  def or(p2) 
    result = [clone()]
    result.push( p2.clone() )
    return PS.new( result )
  end
  def simple_union?(p2)
    return -1 if @p1.length != p2.length
    index1 = -1
    index2 = -1
    count_not_minus = 0
    @p1.length.times{|i|
      if (@p1[i] & (7-p2[i])) > 0 then
        count_not_minus = count_not_minus + 1
        return -1 if count_not_minus >= 2 
        index1 = i
      elsif @p1[i] != p2[i] then
        index2 = i
      end
    }
    return index1 >= 0 ? index1 : index2
  end
  def simple_union(p2, index)
    previous_value = @p1[index]
    @p1[index] |= p2[index]
    return @p1[index] != previous_value
  end
  def [](i)
    @p1[i]
  end 
  def length
    @p1.length
  end
  def each 
    @p1.each{ |e| yield e }
  end
  def output
    @p1.each{ |e| print( ("000"+e.to_s(2))[-3,3]+ " ") }
    print "\n"
  end
  def <=>(p2)
    return 0 if @p1.length != p2.length
    @p1.length.times{|i| 
      return -1 if @p1[i] < p2[i]
      return 1 if @p1[i] > p2[i]
    }
    return 0
  end
end

class PS
  def initialize(elist)
    @pse = elist
  end

  def and(ps2)
    if ps2.is_a?(P) then
      return self.and( PS.new([ps2]) )
    end
    return if self.p_length != ps2.p_length
    result = []
    self.each{ |p1| 
      next if p1.zero?
      ps2.each{ |p2| 
        next if p2.zero?
        p3 = p1.and(p2)
        result.push(p3) if not p3.zero?
      } 
    }
    if result.length == 0 then
      p_length.times{ |i| result.push(0) }
      result = [P.new(result)]
    end
    return PS.new(result)
  end
  def minus(ps2) 
    if ps2.is_a?(P) then
      return self.minus( PS.new([ps2]) )
    end
    return if self.p_length != ps2.p_length
    return self.and(ps2.neg)
  end
  def or(ps2) 
    if ps2.is_a?(P) then
      return self.or( PS.new([ps2]) )
    end
    return if self.p_length != ps2.p_length
    result = []
    self.each{ |p1| result.push(p1.clone()) }
    ps2.each{ |p2| result.push(p2.clone()) }
    return PS.new(result);
  end
  def neg
    result = nil
    @pse.each{ |e| 
      if result == nil then
        result = e.neg
      else
        result = result.and( e.neg )
      end
    }
    return PS.new([0]) if result == nil
    return result
  end
  def reduce
    return self if @pse.length <= 1
    flag = true
    @pse.sort!
    while flag do
      flag = false
      @pse.length.times{|i|
        (i+1).upto(@pse.length) {|j|
          if @pse[i] != nil and @pse[i].merge?(@pse[j]) then
            @pse[i] = @pse[i].merge_or(@pse[j])
            @pse.delete @pse[j]
            flag = true
          end
        }
      }
      @pse.length.times{|i|
        @pse.length.times{|j|
          next if i == j 
          index = @pse[i].simple_union?(@pse[j])
          if index >= 0 then
            if @pse[i].simple_union(@pse[j], index) then
              flag = true
            end
          end
        }
      }
    end
    @reduction_doing = false
    return self
  end

  def content
    return @pse
  end

  def p_length
    return 0 if @pse.length == 0
    @pse[0].length
  end
  def each
    @pse.each{|e| yield e}
  end
  def output
    reduce()
    @pse.each {|p| p.output }
    print "\n"
  end
end

