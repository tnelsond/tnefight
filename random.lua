require("math")
math.randomseed(seed)
setname("Random")
width = math.random()
height = math.random() * 4
setsize(width, height)
setcolor(
	math.random(0, 0xFFFFFF),
	math.random(0, 0xFFFFFF),
	math.random(0, 0xFFFFFF),
	math.random(0, 0xFFFFFF))
setskin(
	math.random(0, 3),
	math.random(0, 3),
	math.random(0, 3),
	math.random(0, 3))
setbruiserness(math.random(0, 100))
setjump(math.random(0, 100));
for i=0,8 do
	setmove(i, math.random(1, 100), math.random(1, 100), math.random(1, 100), math.random(1, 100), math.random(1, 100), math.random(-100, 100), math.random(-100, 100), math.random(1, 100), math.random(1, 100), math.random(0, 360), math.random(0, 360), math.random(1, 100), ATTACK, math.random(0, 12))
end
