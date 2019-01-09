#!/usr/bin/env python3

from multiprocessing import Process

def count(n):
    while n > 0:
        n -= 1

t1 = Process(target=count,args=(10**8,))
t1.start()
t2 = Process(target=count,args=(10**8,))
t2.start()
t1.join(); t2.join()
