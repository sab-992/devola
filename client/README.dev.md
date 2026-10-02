# Devola

## Tech stack

- **Angular (Typescript)**
- **C++**
- **Python**

## Project structure

```
client/
├── src/
│   ├── app/
│   │   ├── classes/
│   │   │   └── ...
│   │   ├── components/
│   │   │   └── ...
│   │   ├── guards/
│   │   │   └── ...
│   │   ├── models/
│   │   │   └── ...
│   │   ├── services/
│   │   │   └── ...
│   │   └── utility/
│   ├── config
│   ├── environments
│   └── ...
└── ...
```

## Getting started

Start by installing the necessary packages

```bash
npm install
```

Then start the development server:
```bash
ng serve --ssl
```

It is also possible to build the project:
```bash
ng build
```
This will compile your project and store the build artifacts in the `dist/` directory. By default, the production build optimizes your application for performance and speed.

## Running unit tests

```bash
ng test
```