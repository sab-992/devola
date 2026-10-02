# Devola

## File structure

```
server/
├── CMakeLists.txt
├── doc/
│   └── ...
├── docker/
│   └── ...
├── service/
│   ├── service_A/
│   │   ├── database/
│   │   ├── docker/
│   │   ├── include/
│   │   │   └── ...         # Contains .hpp files
│   │   ├── settings/
│   │   ├── src/
│   │   │   └── ...         # Contains .cpp files
│   │   ├── CMakeLists.txt/
│   │   └── main.cpp/
│   └── service_B/
│       └── ...
├── settings/
│   └── secrets             # Contains server's certificate and private key (Can be symlinks)
│       ├── cert.pem
│       └── key.pem
└── main.cpp
```