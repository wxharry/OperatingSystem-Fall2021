You should continue processing after encountering an error/warning (other  than a syntax error) and you should be able to 
detect multiple errors in the same run. 
- [ ] 1.  You  should  stop  processing  if  a  syntax  error  is  detected  in  the  input,  print  a  syntax  error message  with  the  line number and the character offset in the input file where observed. A syntax error is defined as a missing token (e.g. 4 used symbols are defined but only 3 are given) or an unexpected token. Stop processing and exit. 
- [ ] 2.  If a symbol is defined multiple times, print an error message and use the value given in the first definition. The error message is to appear as part of printing the symbol table (following symbol=value printout on the same line). 
- [ ] 3.  If a symbol is used in an E-instruction but not defined anywhere, print an error message and use the value absolute zero. 
- [ ] 4.  If a symbol is defined but not used, print a warning message and continue. 
- [ ] 5.  If an address appearing in a definition exceeds the size of the module, print a warning message and treat the address given as 0 (relative to the module). 
- [ ] 6.  If an external address is too large to reference an entry in the use list, print an error message and treat the address as immediate. 
- [ ] 7.  If a symbol appears in a use list but is not actually used in the module (i.e., not referred to in an E-type address), print a warning message and continue. 
- [ ] 8.  If an absolute address exceeds the size of the machine, print an error message and use the absolute value zero. 
- [ ] 9.  If a relative address exceeds the size of the module, print an error message and use the module relative value zero 
(that means you still need to remap “0” that to the correct absolute address). 
- [ ] 10.  If an illegal immediate value (I) is encountered (i.e. >= 10000), print an error and convert the value to 9999.  
- [ ] 11.  If an illegal opcode is encountered (i.e. op >= 10), print an error and convert the <opcode,operand> to 9999. 
 
The following exact limits are in place. 
- [ ] a)  Accepted symbols should be upto 16 characters long (not including terminations e.g. ‘\0’), any longer symbol names 
are erroneous. 
- [ ] b)  a uselist or deflist should support 16 definitions, but not more and an error should be raised. 
- [ ] c)  number of instructions are unlimited (hence the two pass system), but in reality they are limited to the machine size. 
- [ ] d)  Symbol table should support at least 256 symbols (reference program supports exactly 256 symbols). 