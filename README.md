# Molecular dynamics

An exercise for a C/C++ module at ESPCI Paris. The goal is to simulate a system of N particles using event-driven programming. The simulation is done in two dimensions. We consider N disks in a box. These particles are "hard": the collisions between them and with the walls are elastic.

## Getting started

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

SFML 2.4 is needed to build the project.

- Linux:
```
sudo apt-get install libsfml-dev
```
- macOS (if you're using Homebrew):
```
brew install sfml
```

### Installing

Clone the project, then
```
cd molecular-dynamics/
mkdir bin
make
```

## Running a simulation

The program takes two arguments: the particles' radius and the empty space between the particles.

Run the simulation with:
```
./bin/mdsim radius spacing
```

## Authors

- **Samuel Diebolt** - <samuel.diebolt@espci.fr>

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

- Done with help from [_Algortihms, 4th Edition_](https://algs4.cs.princeton.edu/home/).
