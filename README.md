# txtmacro
A text macro compiler.  

## Document
Compile txtmacro with `c++ macro.cpp -o txtmacro.out`, then create a `hello.txt` file and type the content below:
``` hello.txt
.macro hello
hello, world
.endm
hello
```
Run `./txtmacro.out hello.txt`, and you will see `hello, world` in the terminal. Note that `hello.txt` and all main source files must be in the current directory.  
You can use `-i` to replace content in-place or `-o` to output to a specific file.  
This is a free compiler. You can change the keyword style with `-f 0/1`. `0` stands for the `.` style (e.g., `.macro`), and `1` stands for the `#` style (e.g., `#macro`). If you are not satisfied with these, use `--flag` to customize the style you need.  
You can also check information by using `--help`.  

### Syntax
```
.macro name
...
.endm
```
The `.macro`keyword is used for multi-line definitions. It requires a name and must end with `.endm`.  
```
.define name [...]
```
The `.define` keyword is used for single-line definitions. It requires a name.  
```
.repeat name times word
```
The `.repeat` keyword repeats `word` `times` times.  
```
.include name
```
The `.include` keyword reads and inserts the content of the `name` file.  
```
.import name
```
The `.import` keyword reads the `name` file but does not insert content.  
