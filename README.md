# Devola

An AI-powered job listing web app. Candidates can browse and filter job listings, manage the RSS/job feeds they're subscribed to, and upload resumes (tagged with a name and skills) that are used to generate scored job recommendations.

![Devola main page](/settings/assets/devola_main_page.png)

## Table of Contents

- [Features](#features)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
  - [Required](#required)
  - [Pulled Automatically](#pulled-automatically)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Environment Configuration](#environment-configuration)
  - [Client Setup](#client-setup)
  - [Server Setup](#server-setup)
- [Building](#building)
- [Running Tests](#running-tests)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Browse and filter** job listings
- **Manage feeds**: subscribe to and unsubscribe from RSS/job feeds
- **Resume uploads**: tag each resume with a name and skills
- **Scored recommendations**: job listings ranked against your resume

## Tech Stack

- **Client:** Angular (TypeScript)
- **Server:** C++20
- **Tooling:** Python 3.14 (`devola.py` project script)
- **Database / cache:** PostgreSQL, Redis
- **Infrastructure:** Docker, nginx

![Architecture](/settings/assets/architecture.png)

## Project Structure

```
devola/
├── client/
│   └── ...                     # For details see client/README.dev.md
├── cmake/
├── docker/
├── lib/
│   ├── CMakeLists.txt
│   ├── lib_a/
│   │   ├── include/
│   │   │   ├── lib_a/
│   │   │   │   ├── module_a/   # Contains .hpp files
│   │   │   │   │   ├── detail/
│   │   │   │   │   │   └── ...
│   │   │   │   │   └── ...
│   │   │   │   └── ...
│   │   │   └── lib_a.hpp       # Umbrella header
│   │   └── src/
│   │       ├── module_a/       # Contains .cpp files
│   │       │   ├── detail/
│   │       │   │   └── ...
│   │       │   └── ...
│   │       └── ...
│   └── ...
├── nginx/
├── server/
│   └── ...                     # For details see server/README.dev.md
├── settings/
│   ├── command/
│   ├── .env                    # Environment file for the project
│   └── ...
├── test/
│   ├── CMakeLists.txt
│   ├── lib_a/
│   │   ├── module_a/
│   │   │   ├── test.cpp
│   │   │   └── ...
│   │   └── ...
│   └── ...
├── CMakeLists.txt
├── devola.py
├── LICENSE
├── README.md
└── requirements.txt
```

## Dependencies

### Required

- Python 3.14
- A C++20-capable compiler
- CMake (3.20+)
- Node.js and npm
- PostgreSQL
- Redis
- Docker (mainly for Windows)

### Pulled Automatically

These are fetched by the build, so you don't need to install them yourself:

- [asio](https://think-async.com/Asio/)
- [libpqxx](https://github.com/jtv/libpqxx)
- [hiredis](https://github.com/redis/hiredis)
- [libsodium](https://github.com/jedisct1/libsodium)
- [jwt-cpp](https://github.com/Thalhammer/jwt-cpp)
- [pugixml](https://pugixml.org/)
- [nlohmann-json](https://github.com/nlohmann/json)

## Getting Started

### Prerequisites

Make sure everything under [Required](#required) is installed and that PostgreSQL and Redis are running.

Clone the repository:

```bash
git clone https://github.com/sab-992/devola.git
cd devola
```

### Environment Configuration

**The project needs a valid certificate from** `server/settings/secrets/cert.pem` **and a private key from** `server/settings/secrets/key.pem` **to launch the server using TLS/SSL.**

The project also reads its configuration from `settings/.env`.

This is what the .env file should look like:

```bash
PGADMIN_DEFAULT_EMAIL=...
PGADMIN_DEFAULT_PASSWORD=...


JWT_SECRET=...
POSTGRES_APP_PASSWORD=...
REDIS_APP_PASSWORD=...
```

**POSTGRES_APP_PASSWORD**, **JWT_SECRET** and **REDIS_APP_PASSWORD** are all passwords you **NEED** to define (try to use a high entropy password).

*PGADMIN_DEFAULT_EMAIL* is the email used for PgAdmin and *PGADMIN_DEFAULT_PASSWORD* is the password (devs only). This is only available if you launch the dockers in development mode:

```bash
python devola.py deploy
```

### Client Setup

From the `client/` directory, install the packages:

```bash
cd client
npm install
```

Then start the development server:

```bash
ng serve --ssl
```

If the client launched successfully, the output should look like this:

![Terminal output of a successful `ng serve --ssl`](/settings/assets/ng_serve_success.png)

### Server Setup

Create a virtual environment and install Python requirements:

```bash
python -m venv .venv
pip install -r requirements.txt
```

Then start the development server:

```bash
source ./.venv/bin/activate
python devola.py makeall -l
```

If the server launched successfully, the output should look like this:

![Terminal output of a successful `ng serve --ssl`](/settings/assets/server_started_.png)


If you want details on the commands available in **devola.py**, use this:

```bash
python devola.py --help
```

## Building

To build the client:

```bash
cd client
ng build
```

This compiles the project and stores the build artifacts in the `dist/` directory. By default, the production build is optimized for performance and speed.

To build the server:

```bash
source ./.venv/bin/activate
python devola.py build
```

## Running Tests

Client tests:

```bash
cd client
ng test
```

Server tests:

```bash
source ./.venv/bin/activate
python devola.py maketest -l
```

## Contributing

Devola is a personal project, so I'm not actively looking for contributions. You're welcome to fork the repository and make it your own under the terms of the GPL-3.0. If you've built something you think is worth merging, feel free to open a pull request, but I can't promise I'll review or accept it.

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE) (GPL-3.0). See the [LICENSE](LICENSE) file for details.