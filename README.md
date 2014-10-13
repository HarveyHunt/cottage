cottage
========
[![Build Status](https://travis-ci.org/HarveyHunt/cottage.svg?branch=develop)](https://travis-ci.org/HarveyHunt/cottage)


###Use howm's commands and operators through a UNIX socket.

Contents
========
* [Configuration] (#configuration)
* [Usage] (#usage)
* [Errors] (#errors)

## Configuration

Configuration is extremely minimal and is done from within the cottage source file.

* **SOCK_PATH**: The path to where howm's UNIX socket is.

```
#define SOCK_PATH "/tmp/howm"
```

* **BUF_SIZE**: The size of the sending buffer.

```
#define BUF_SIZE 1024
```

## Usage

Cottage should be used in the following manner:

```
cottage [command/operator] <args>
```

## Errors

Upon error, howm will return a response  code. Cottage will print a relevant error message and return the error number to its caller.
