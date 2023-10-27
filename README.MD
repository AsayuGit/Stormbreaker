
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

#### To build the table from a world list:
```bash
  ./stormbreaker -G -i rockyou.txt -o mytable.table
```

#### To simply hash a world list:
```bash
  ./stormbreaker -G -m -i rockyou.txt -o hashes.txt
```

#### To match an hashlist with a previously generated table:
```bash
  ./stormbreaker -L -t mytable.table -i hashes.txt -o matches.txt
```

#### To only get the password from an hashlist:
```bash
  ./stormbreaker -L -m -t mytable.table -i hashes.txt -o passwords.txt
```

#### With any of the above commands it is possible to substitute the "-i" argument for the standard input like so:
```bash
  cat rockyou.txt | ./stormbreaker -G -o mytable.table
```

#### Likewise if you omit the "-o" option, stormbreaker will output to the console.
```bash
  cat rockyou.txt | ./stormbreaker -G
```

#### To change the hashing algorithm use the "-a" option:
```bash
  cat rockyou.txt | ./stormbreaker -G -o mytable.table -a MD5
```

#### By default, stormbreaker is multithreaded and will take advantage of all available threads. Should you with to alter it, use the "-j" option to set the number of threads to use:
```bash
  cat rockyou.txt | ./stormbreaker -G -o mytable.table -a MD5 -j 4
```
- Do note that a number of theads of 0 will be interpretes as (All available threads) and thus have no effect over the default.
- Another side effect of multithreading is that the output may be out of order compared to the input. This doesn't affect the validity of the output but if you require the output to be in the same order as the input (to use diff per exemple) then restrict the number of threads to 1 "-j 1".

#### You can call stormbreaker with only "-G" or "-L" which will allow you to enter the input by hand. To end your input simply press CTRL+d :
```bash
  ./stormbreaker -G
  passwordA
  password B
  
  password D
  CTRL+d
```
  output:
```
  INFO: 4 hashes written.
  577e1ad5d48123667d6272f2e026e119d8d9391f6fd28ab08944b87a12e41a68:passwordA
  fe64c4a82f071440f5e70d373ca3f5c83483a0d351650f62d7a0af31af6f74ea:password B
  68f398eee9dcff37d85416d8da3f347be1666cb6112c58bda4b6f511f94deca2:password D
  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855:
```

## Dependencies

In order to be able to build stormbreaker the following dependencies are needed:

### For the docker build:
- docker.io

### For the local build:

- libssl3
- libssl-dev
## Docker Build

To compile & run in a docker container run:

```bash
  cd stormbreaker
  make docker-build
  make docker-run
  stormbreaker -G -i rockyou.txt -o mytable.table
```

A shell from within the newly build docker container will open in the current working directory.

#### /!\ You should not prefix stormbreaker with "./" while in the docker container as stormbreaker will be available in the path, and located at "/usr/local/bin/stormbreaker"

- Do note that you may need to run these commands as root depending on your docker setup.


## Local Build

To compile & run locally run

```bash
  cd stormbreaker
  make
  ./stormbreaker -G -i rockyou.txt -o mytable.table
```
## Authors

- [@AsayuGit](https://github.com/AsayuGit)

## Testing

To test the good workings of stormbreaker, a shell script is available "diag.sh."

When ran diag.sh will test the various modes of stormbreaker and return "SUCCESS !" if every test succeed, or an error with its description.

## FAQ

#### What is the recommended way to build stormbreaker ?

The recommended way to build stormbreaker would be via the docker build.
This ensure that all the dependencies for the app will be met.

#### Can stormbreaker work without without generating a dictionary ?

No, it is imperative to generate the dictionary from a worldlist before looking up hashes.

#### What are the supported OS for stormbreaker ?

stormbreaker has been build targetting GNU/Linux but it should be able to run on any OS you with to build it for.

#### How can I tell if stormbreaker is running correctly ?

stormbreaker comes with a diagnostic shell script "diag.sh", run it and if everything goes well it should return "SUCCESS !".