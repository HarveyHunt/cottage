cottage
========
[![Build Status](https://travis-ci.org/HarveyHunt/cottage.svg?branch=develop)](https://travis-ci.org/HarveyHunt/cottage)


###Use howm's commands, operators and set configuration values through a UNIX socket.

Contents
========
* [Installation](#installation)
* [Configuration] (#configuration)
* [Usage] (#usage)
* [Errors] (#errors)

##Installation
Cottage is on the [AUR](https://aur.archlinux.org/), there are two packages for it:
* [cottage-git](https://aur.archlinux.org/packages/cottage-git/) is the bleeding edge package.
* [cottage](https://aur.archlinux.org/packages/cottage/) is the package based off of stable releases.

If you can't use the AUR, then there may be a package available for your distro.

As a last resort, do the following:

```
git clone https://github.com/HarveyHunt/cottage
cd cottage
make
sudo make install
```

## Configuration

Configuration is extremely minimal and is done from within the ```cottage``` source file or by setting environment variables.

In order to change the socket that ```cottage``` attempts to connect to, modify the environment variable ```HOWM_SOCK```. For example:

```
export HOWM_SOCK=/tmp/howm_test
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
cottage -f count 2
cottage -f motion w
```

## Errors

Upon error, howm will return a response  code. Cottage will print a relevant error message and return ```EXIT_FAILURE```.
