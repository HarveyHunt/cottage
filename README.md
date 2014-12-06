cottage
========
[![Build Status](https://travis-ci.org/HarveyHunt/cottage.svg?branch=develop)](https://travis-ci.org/HarveyHunt/cottage)


###Use howm's commands, operators and set configuration values through a UNIX socket.

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

Changing howm's config value is done in the following manner:

```
cottage -c config_var value
```

Calling one of howm's functions is done in the following manner:

```
cottage -f function_name <args>
```

It is possible to call operators using cottage, but it requires you call functions to set the operator, the count and finally the motion. Here is an example:

```
cottage -f op_kill
cottage -f set_count 2
cottage -f motion w
```

## Errors

Upon error, howm will return a response  code. Cottage will print a relevant error message and return the error number to its caller.
