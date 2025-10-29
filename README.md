# Adequate-C
A statically-typed C-inspired language with modern features - adequately good.

## Building

#### Build
```bash
make build
```

#### Build and Run
```bash
make run
```

#### Other Commands
```bash
make clean   # Remove build dir
make format  # Format source code
make help    # Show all commands
```

#### Manual Build (without Makefile)
```bash
mkdir build && cd build
cmake ..
make
./adequatec
```

## Requirements
- C++17
- CMake 3.10+
- clang-format (for code formatting)
