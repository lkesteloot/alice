# bg80d

## What is it

A simple Z80 disassembler, built to support my [Ms. Pac-Man Disassembly](http://bartgrantham.com/projects/ms-pacman-disassembly/) project.

**WARNING** : This is beta code.  It works well for my limited use, but it's not particularly well-tested.

## What's special about it?

Instead of attempting to decode the opcode in code (heh) I broke the Z80 opcode specifications into separate files, `*.dat` in the repo.  Because of CB/DD/ED/FD opcode prefixes there's still some decoding that happens on the fly, but the operands for a given instruction are just a table lookup.  This allows the disassemler to be small, **less than 200 lines**, most of which is parameter/input file parsing.

Also, it has some features that I've found helpful in my documenting old Z80 code.  Code and data tend to be intertwined pretty badly in at least the Pac-Man ROM so I included the capability to specify ranges to decode/not decode, mixing in a file of annotations, and some other tricks.

Finally I include, _as far as I know_, all the undocumented opcodes.  7 tables of prefixes * 256 = 1792 opcodes.

## How do I use the python script?

`cat some_z80_rom.bin | ./bg80d.py`

You can pass in various input files that help bg80d understand how you want it to interpret the file.  In short, the range/skip/stride files have the following format:

```
# comments on lines by themselves are ignored, as are blank lines

0-7       # comments after rangespecs become part of the annotation stream
$68-$7c   # you can use one decimal format and two hex formats for memspecs:  1000 or $3e8 or 0x3e8
0x7D-$8c  # the two hex formats are case-insensitive, and you can mix and match (probably not a good idea)
```

Annotation files are any parsable memspec in the first column (1000 or $3e8 or 0x3e8) on a line by itself, followed by annotations.  The annotation(s) will be output immediately before the decoded memory location:

```
0
;;; RST0 - __start()
;;; Turns off interrupts, sets the interrupt table to $3F00, and jumps to boot()
8

;;; RST8 - memset(HL, A, B)
;;; Fill (HL)...(HL+B) with Accumulator
```

## How do use the C-based decoder?

The C-based decoder function doesn't have the same functionality, but it will assist in debugging z80 code from a C emulator/disassembler/analyzer.  It's pretty simple to use:

Just `#include "bg80d.h"`, and then call `decode()` with a function pointer that reads and returns bytes, a `void *` for context for your function, and an `int` for the current PC (not critical, can be 0).  It will return an `opcode_spec` structure that has information about how that opcode was decoded (see bg80d.h for fields).

The function you pass in must have the signature `__uint8_t myfunc(void *)`.  `decode()` will use this function, calling it with the supplied `void *`,  to obtain a stream of bytes as needed to determine prefix and nn/n/d/dn parameters.

If you pass in PC, it will include the post-decoded PC in `opcode_spec->pc_after` (if you pass in 0, you'll simply get the number of bytes read).  It will also use the supplied PC to compute relative jumps for `opcode_spec->description`.

Example program:

```c
#include "bg80d.h"

__uint8_t rom[17] = {0xf3, 0x3e, 0x3f, 0xed, 0x47, 0xc3, 0x0b, 0x23, 0x77, 0x23, 0x10, 0xFC, 0xc9, 0xc3, 0x0e, 0x07, 0x00};

int addr = 0;

__uint8_t romread(void * ctx) {
    __uint8_t rval = rom[*(int *)ctx];
    (*((int *)ctx))++;
    return rval;
}

int main() {
    int PC = 0;
    opcode_spec_t * opcode_spec;

    do {
        opcode_spec = decode(romread, (void *)(&addr), PC);
        printf("%d\t%s\t%.2X\t%s\n", PC, opcode_spec->prefix, opcode_spec->opcode, opcode_spec->description);
        PC = opcode_spec->pc_after;
    } while ( opcode_spec->opcode != 0x00 );
}
```



- - - -


Other than that, for now you'll have to make do with `./bg80d.py --help`


- - - -

## TODO

* WTH am I doing with bytestream and stream_bytes?... duh!
* better documentation!
* allow user to provide formatting string
  * ie. "%-5s{addr} $%.4X{addr}   %-6s{opcode} %-15{mnemonic} %-11{param} ;  %{desc}"
  * addr/opcode/operands in dec/hex/HEX
  * mnemonic (w or w/o operand substitutions)
  * description (w or w/o operand substitutions)
  * opcode_type, opcode_dest, opcode_src, opcode_summary 
* stride code could be cleaner
* annotation parsing could be cleaner
* does strides get decoded in an expected way WRT start/end vs. ranges/skips?
* possible that the ddcbnn opcodes are buggy
* How about a labels.txt for better integration with asms?
  * Could do addr->label replacement on output


## License

[MIT License](http://www.opensource.org/licenses/mit-license.php)


Enjoy!

-Bart
