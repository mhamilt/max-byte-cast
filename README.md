# byte-cast

byte-cast is a MaxMSP utility object for casting a list of bytes to other data formats. Useful when interacting with Arduino using the [`serial` object](https://docs.cycling74.com/max8/refpages/serial).


## Install

[Un-zip the package folder](https://github.com/mhamilt/max-byte-cast/releases/download/0.1-alpha.1/byte-cast.zip) into you `~/Documents/Max 8/Packages` directory.

## Usage

This release covers all the standard data types:

| Format | Data Type             |
| ------ | --------------------- |
| i      | 32-bit int            |
| h      | 16-bit int            |
| f      | 32-bit float          |
| s      | ASCII string (greedy) |

**Modifiers:**

-   `u`:   unsigned     (`i` and `h` only)
-   `>`:   Big endian    (append)
-   `<`:   Little endian (append)

**Endianness Toggle:**

`0` or `1` in the right inlet will flip the endianness.

### Behaviour

The string format will greedily use up the entire byte list and attempt to translate to ASCII characters until the end. Bytes outside of the symbol range 32 - 126 will be replaced with a `' '` character.

The number formats will send out a list for as many complete numbers that are available. Any incomplete numbers will be ignored.

**e.g.** if you send 7-bytes when in float format, a single number will be output. If you send 9-bytes, 2 numbers will be output.
