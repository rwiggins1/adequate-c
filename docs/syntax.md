## Quick Reference

### Variable Declaration
```cpp
var int x = 5;           // Explicit type (requires `var`)
infer y = 10;            // Type inference (`infer` implies `var`, omit it)
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
x && y    x || y    !x

// Assignment
x = 5;
```

### Data Types
```cpp
var int x = 42;
var float f = 3.14;
var double d = 3.14159;
var bool flag = true;
var char c = 'a';
var string s = "hello";
```
## Comments

### Single-line Comments
```cpp
// This is a single-line comment
var int x = 5;  // Comment after code
```

### Multi-line Comments
```cpp
/* This is a multi-line comment
   spanning multiple lines */
var int y = 10;

/* Can also be on one line */
```

### Rules
- Comments are ignored by the compiler
- Single-line comments start with `//` and continue to end of line
- Multi-line comments start with `/*` and end with `*/`
- Multi-line comments cannot be nested

## Notes
Not all of these syntax features have been added. But they are planned for the initial release.
