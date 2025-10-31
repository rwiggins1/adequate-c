## Quick Reference

### Variable Declaration
```cpp
int x = 5;              // Explicit type
infer y = 10;           // Type inference
```

### Function
```cpp
func name(int param) -> int {
	return param * 2;
}
```

### If-Else
```cpp
if (condition) {
	// code
}
else {
	// code
}
```

### While Loop
```cpp
while (condition) {
	// code
}
```

### Function Call
```cpp
result = functionName(arg1, arg2);
```

### Operators
```cpp
// Arithmetic
x + y    x - y    x * y    x / y    x % y

// Comparison
x == y   x != y   x > y    x < y    x >= y   x <= y

// Logical
x & y    x | y    !x

// Assignment
x = 5
```

### Data Types
```cpp
int x = 42;
float f = 3.14;
double d = 3.14159;
bool flag = true;
char c = 'a';
string s = "hello";
```

## OOP (Future)
### Classes
```cpp
class className {
private:
    int x;

public:
    // constructor with initialization
    className(int x): x(x) {}

    func getX() -> int {
        return x;
    }

    ~className() {}
}

// access getX
className::getX();
```

## Notes
Not all of these syntax features have been added. But they are planned for the initial release.
