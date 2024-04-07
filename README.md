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
`__del__` is a finalizer and is called right before object is about to be deallocated.
This however, does not guarantee that object was deallocated. Idk patches python in runtime to be able to hook into actual deallocation 
process and calls your callback right after deallocation routine of object is called. 
Callback receives 1 parameter which is id of just deallocated object. Important to note that it is still possible to have object alive after its deallocation routine
was called in some scenarios under certain circumstances.
This package is meant for education/debugging purposes.

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

### Important
This is guaranteed to work only on linux/x86_64 and Python 3.9.0. Other configurations were not tested. Use it at your own risk.
