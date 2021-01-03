# WonderSwan Disassembler
A wrapper for the x86 disassembler included in [bochs](http://bochs.sourceforge.net) aimed at WonderSwan roms.
## Usage
Disassemble bank 0x1f to ouput.txt
```
wsdisasm -i rom.ws -b 1f
```
Disassemble an already extracted bank to disasm.txt
```
wsdisasm -i bank_1f.bin -o disasm.txt
```
Disassemble the first 0x100 bytes of bank 0x1a
```
wsdisasm -i rom.ws -b 1a -l 100
```
Check the help for more information
```
wsdisasm -h
```
