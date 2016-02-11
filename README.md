# rec-rsl
Receive RSL 433MHz order code

![Stable version](https://img.shields.io/badge/stable-1.2.0-blue.svg)
[![BSD-3 license](https://img.shields.io/badge/license-BSD--3--Clause-428F7E.svg)](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29)

# How To Compile

Steps are to perform directly on the [Raspberry Pi](https://www.raspberrypi.org/products/):

* Get [wirinPi](http://wiringpi.com/) library
```bash
# Create directory which will host sources
mkdir -p ~/src
# Move into this directory
cd ~/src
# Get sources
git clone git://git.drogon.net/wiringPi
# Move into wiringpi directory
cd ~/src/wiringpi
# Build the library
./build
```
* Get [rec-rsl](https://github.com/cyosp/rec-rsl) repository
```bash
# Move into sources directory
cd ~/src
# Get sources
git clone https://github.com/cyosp/rec-rsl
```
* Compile and install [rec-rsl](https://github.com/cyosp/rec-rsl)
```bash
# Move into sources directory
cd ~/src/rec-rsl
# Compile rec-rsl
make
# Install rec-rsl
sudo make install
```

# Usage

## Command line

```bash
sudo rec-rsl <pin number> [file to write codes]
```

Arguments detailed:
 * pin number

	[Raspberry Pi](https://www.raspberrypi.org/products/) pin following [wirinPi](http://wiringpi.com/) implementation
 * file to write codes

	Absolute file path where codes are written instead of stdout

## Examples

```bash
# Receive RSL order codes with a 433MHz receiver connected to pin number 2
sudo rec-rsl 2
# Receive RSL order codes with a 433MHz receiver connected to pin number 2 and store them into /tmp/rec-rsl.log file
sudo rec-rsl 2 /tmp/rec-rsl.log
```
## License

**[rec-rsl](https://github.com/cyosp/rec-rsl)** is released under the BSD 3-Clause License. See the bundled `LICENSE.md` for details.