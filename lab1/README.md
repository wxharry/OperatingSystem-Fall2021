# pass 2 linker

## How to Run
use `make` command to build and run the executable files.
You can specifiy the input file as `./bin/xw27882passlinker ./lab1_assign/input-1`
The results are required to use standard io.

## How to Test
```
cd lab1_assign/
./runit.sh <your_output_directory>/ ./<you_executable_file> # generate output
./gradeit.sh . ./<your_output_directory>/ # compare you output with the given output

```


## Write at the end

This is the hardest programming I have ever done. 

Honestry, it is not difficult to understand what our pass 2 linker need to do. The framwork of the program is also easy to construct and professor illustrated a psudocode of one pass procedure. 

Here is the most hardest thing: *Error and warning detection* and *display in a specific order*. That truly makes things complicated. It took me several hours to program the linker, and I stayed up all night long to accomplish these (mostly because the due is today though, I admit). 

I want to clarify that I am not complaining about the complex lab1, but sharing my feelings of this experience. 

I had to admitted that it was painful, but it was like a climb. What you only can do is to move step by step to witness the great view on the top.

Though, this project is sooo time-limited. For me, I spent a lot of figuring out what the program can do, what is the connection between the inputs and outputs. So, that gave me very little time for coding, I will try to hurry up next time. 
