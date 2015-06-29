require "./libgeocalc"

# sample
ps1 = PS.new([])
ps2 = PS.new([])

p1 = P.new(["001","010","011"])
p2 = P.new(["001","011","001"])

p3 = p1.or(p2)

#p3.output
p3.neg.or(p3).output
p3.neg.and(p3).output

p1.neg.minus(p2).output


