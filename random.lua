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
--setmove(0, 1, 1, 0);
--setmove(1, 10, 0, -1.0);
--setmove(2, 20, 0.4, 0.3);
