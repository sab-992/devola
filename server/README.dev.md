# devola/server

## File structure
```
devola/
├── app/
│   ├── main.cpp
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── private_module_a/
│   │   │   ├── file2.h
│   │   │   └── ...     (Other private module headers)
│   │   └── ...     (Other private modules)
│   ├── src/
│   │   ├── private_module_a/
│   │   │   ├── file2.cpp
│   │   │   └── ...     (Other private module files)
│   │   └── ...
│   └── tests/
│       ├── private_module_a/
│       │   ├── test1.cpp
│       │   └── ...     (Other private module files)
│       └── ...
├── client/
├── cmake/
├── database/
├── docker/
├── libs/
│   ├── CMakeLists.txt
│   ├── lib_a/
│   │   ├── include/
│   │   │   ├── lib_a/
│   │   │   │   ├── module_a/
│   │   │   │   │   ├── detail/
│   │   │   │   │   ├── file1.h
│   │   │   │   │   └── ...     (Other module headers)
│   │   │   │   └── ...     (Other modules)
│   │   │   └── lib_a.h     (Umbrella header)
│   │   └── src/
│   │       ├── module_a/
│   │       │   ├── file1.cpp
│   │       │   └── ...     (Other module files)
│   │       └── ...     (Other modules)
│   └── ...     (Other libraries)
├── nginx/
├── server/
│   ├── CMakeLists.txt
│   └── service/
│       ├── service_a/
│       │   ├── main.cpp
│       │   ├── CMakeLists.txt
│       │   ├── include/
│       │   │   ├── private_module_b/
│       │   │   └── ...
│       │   ├── src/
│       │   └── tests/
│       └── ...
├── settings/
│   ├── command/
│   ├── .env
│   └── ...
├── tests/
│   ├── CMakeLists.txt
│   ├── lib_a
│   │   ├── module_a/
│   │   │   ├── test2.cpp
│   │   │   └── ...
│   │   └── ...
│   └── ...
├── CMakeLists.txt
├── devola.py
├── README.md
└── requirements.txt
```