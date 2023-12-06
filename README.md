# IDK
### I dont know how to name this package.
Hook into process of python object deallocation.

```python
import idk


def callback(obj_id):
    print(f"object {obj_id} was deallocated")


x = "delete me"
print(id(x))
idk.watch(x, callback)
del x
```

Dont confuse object deallocation and `__del__` dunder method. 
`__del__` deletes reference to the object and even if last variable 
that references it is deleted it does not guarantee that object will 
actually be deallocated immediatelly after calling `del` as it may contain inner cycles and
be deallocated later when garbage collector runs or not be deallocated at all if interpreter is shutting down and garbage collector did not run.

```python
import idk


def callback(obj_id):
    print(f"object {obj_id} was deallocated")


x = []
x.append(x)
print(id(x))
idk.watch(x, callback)
del x
print("this line is printed before object is deallocated")
```

Idk patches python in runtime to be able to hook into actual deallocation 
process and calls your callback right after object was deallocated. 
Callback receives 1 parameter which is id of just deallocated object.
This package is meant for education/debugging purposes.

### Important
This is guaranteed to work only on linux/x86_64 and Python 3.9.0. Other configurations were not tested. Use it at your own risk.
