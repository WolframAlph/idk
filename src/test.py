import idk

def cb(obj):
    input(f"{obj} was deallocated")

while True:
    x = []
    print(id(x))
    x.append(x)
    idk.watch(x, cb)
