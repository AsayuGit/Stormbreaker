
# Stormbreaker

A simple command line utility to break hashes and retrive their assotiated passwords.


[![GPLv2 License](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://opensource.org/licenses/)
## Features

- Generate a dictionary from a word list
- Fast Lookup of an hash to retrieve its password
- Compatible with all OpenSLL's hashing algorigthm
- Multithreaded
- Cross platform


## Usage/Examples

#### Syntax
```bash
  stormbreaker: [mode] [options]
```

#### Exemple
```bash
  EG: ./stormbreaker -G -a SHA256 -o rainbow.table
```

#### Options
|  Option   |   Alt         | Description                                     |
| :-------- | :------------ | :---------------------------------------------- |
| `-h`      | `--help`      | Display this help table                         |
| `-G`      | `--Generate`  | Generate a hash table                           |
| `-L`      | `--Lookup`    | Lookup an hash to find its originating password |
| `-o`      | `--output`    | Set the output file for the operation           |
| `-i`      | `--input`     | Set the input file for the operation            |
| `-t`      | `--table`     | Set the table file in L mode                    |
| `-a`      | `--algorithm` | Set the algorithm used for generation           |
| `-j`      | `--jobs`      | Set the number of threads to use for compute    |
| `-m`      | `--minimal`   | Reduce the output for testing purposes          |

## Dependencies

In order to be able to build stormbreaker the following dependencies are needed:

### For the docker build:
- docker.io

### For the local build:

- libssl3
- libssl-dev
## Docker Build

To compile & run in a docker container run

```bash
  cd stormbreaker
  make docker-build
  make docker-run
```

A shell from within the newly build docker container will open in the current working directory.

Stormbreaker will be available in the path, and located at /usr/local/bin/stormbreaker

- Do note that you may need to run these commands as root depending on your docker setup.


## Local Build

To compile & run locally run

```bash
  cd stormbreaker
  make
  ./stormbreaker
```
## Authors

- [@AsayuGit](https://github.com/AsayuGit)


## FAQ

#### What is the recommended way to build stormbreaker ?

The recommended way to build stormbreaker would be via the docker build.
This ensure that all the dependencies for the app will be met.

#### Can stormbreaker work without without generating a dictionary ?

No, it is imperative to generate the dictionary from a worldlist before looking up hashes.

#### What are the supported OS for stormbreaker ?

stormbreaker has been build targetting GNU/Linux but it should be able to run on any OS you with to build it for.