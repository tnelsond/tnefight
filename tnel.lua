setname("Tnel")
setsize(2, 3)
setcolor(0x44DD00, 0x555500, 0xAA8800, 0x557700)
setskin(2, 1, 3, 0)
setbruiserness(20)
setjump(60);

setmove(0, 20, 30, 0, 5, 100, 10, 0, 5, 5, 0, 45, 40, ATTACK, 12)
setmove(1, 10, 30, 0, 5, 100, 10, 0, 5, 5, 45, 45, 100, ATTACK + MOVEMENT, 12)
setmove(2, 0, 30, 0, 5, 100, 10, 0, 5, 5, 0, 45, 40, REFLECT, 12)
setmove(3, 30, 30, 0, 5, 100, 10, 0, 100, 100, 0, 45, 40, ATTACK, 12)

setmove(4, 50, 0, 0, 20, 70, 0, 0, 1, 1, 0, -20, 100, ATTACK + PROJECTILE, 9)
