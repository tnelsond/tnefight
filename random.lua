require("math")
math.randomseed(seed)
setname("Tnel")
width = math.random(2, 3)
height = width * (1.1 + math.random()*.9)
setsize(width, height)
setcolor(math.random(0, 0xFF), math.random(0, 0xFF), math.random(0, 0xFF))
setskin(math.random(0, 13), math.random(0, 13), math.random(0, 13))
setbruiserness(math.random(0, 100))
setjump(math.random(0, 100));
for i=0,8 do
	setmove(i, math.random(1, 100), math.random(1, 100), math.random(1, 100), math.random(1, 100), math.random() * 4, math.random() * 4, math.random(1, 360), math.random() * 2, math.random(1, 64), math.random(0, 12))
end
