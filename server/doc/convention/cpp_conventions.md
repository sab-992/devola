## C++ Conventions


### File Names
- **Source files**: `snake_case.cpp`
- **Header files**: `snake_case.h`

### Classes
- **Concrete/Template classes**: `PascalCase`
- **Components**: Same as concrete class with suffix `_c`
- **Interfaces/Abstract classes**: Same as concrete class with suffix `_i`

### Variables
- **Public members**: `camelCase`
- **Private members**: `camelCase` with underscore prefix `m_`
- **Local variables** `camelCase`
- **Constants/constexpr/const variables**: `UPPER_SNAKE_CASE`

### Functions and Methods
- **Functions**: `camelCase`

### Enums and Enum Values
- **Enum name**: `PascalCase` with suffix `_en`
- **Enum values**: `UPPER_SNAKE_CASE`

### Namespaces
- **Namespaces**: `lowercase` with suffix `_n`

### Macro Names
- **Macros**: `UPPER_SNAKE_CASE`

### Includes
- **Order**: `Alphabetic`
- **Type**: `#include <...>`