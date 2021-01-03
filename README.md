# WonderSwan Disassembler
A wrapper for the x86 disassembler included in [bochs](http://bochs.sourceforge.net) aimed at WonderSwan roms.
## Usage
Disassemble bank 0x1f to ouput.txt
```
wsdisasm -b 1f rom.ws
```
Disassemble an already extracted bank to disasm.txt
```
wsdisasm -o disasm.txt bank_1f.bin
```
Disassemble the first 0x100 bytes of bank 0x1a
```
wsdisasm -b 1a -l 100 rom.ws
```
Check the help for more information
```
wsdisasm -h
```
