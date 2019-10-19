# pwm4term

A linux terminal with password management.

You need the `vte` package to run this terminal emulator.

## Installation

### Dependencies
* Debian:
`sudo apt install libvte-2.91-dev`

* Archlinux:
`sudo pacman -S vte3`

`mkdir build && cd $_`

`cmake ..`

`make`

`ln -s $(pwd)/term /usr/local/bin`

`term`


## Add passwords

`vim ~/.config/pwm4term.passwords`

```
username@hostname password

user@github password
```

## Usage

When you see a sudo password prompt or credentials for git clone/pulls, press F1. If a password from the list is matched it is inserted followed by a ENTER.
