# CLOX
Interpreter and compiler implemented in modern C++ for the lox programing language described in [Crafting Interpreters](https://craftinginterpreters.com/) with extensions in grammars and other features. 

In a long run_code, this project will support compile to binary and JIT with LLVM.  

![example workflow](https://github.com/SmartPolarBear/clox/actions/workflows/build_and_test.yml/badge.svg)    
[![issues](https://img.shields.io/github/issues/SmartPolarBear/clox)](https://github.com/SmartPolarBear/clox/issues)
[![forks](https://img.shields.io/github/forks/SmartPolarBear/clox)](https://github.com/SmartPolarBear/clox/fork)
[![stars](https://img.shields.io/github/stars/SmartPolarBear/clox)](https://github.com/SmartPolarBear/clox/stargazers)
[![license](https://img.shields.io/github/license/SmartPolarBear/clox)](https://github.com/SmartPolarBear/clox/blob/master/LICENSE)
[![twitter](https://img.shields.io/twitter/url?style=social&url=https%3A%2F%2Ftwitter.com%2F___zirconium___)](https://twitter.com/___zirconium___)

### Built With
#### Environment
This project is built with  

- MSVC 17.0
- CMAKE, 3.19 and above.

**Note:** C++20 support of compilers, especially support for `std::format` and concepts are required for compiling most of the codes. Now, only Visual Studio 16.9 and above are known to support them all. And LLVM 13 may work because it claims its test formatting features are "In progress". Also for this reason, only releases for Windows is provided for the present.  

#### Third-party Libraries
- [microsoft/GSL](https://github.com/microsoft/GSL)  
- [p-ranav/argparse](https://github.com/p-ranav/argparse)  
- [Neargye/magic_enum](https://github.com/Neargye/magic_enum)  
- [google/googletest](https://github.com/google/googletest)    

## Usage  
`clox` to enter REPL mode.    
`clox --file <source file>` to run_code a file.  

## Roadmap  

|Naive CLOX|Typed CLOX|CLOXc
|----------|----------|---------|
✅ Basic language features<br><br> **Release: [First preview version of clox interpreter](https://github.com/SmartPolarBear/clox/releases/tag/v0.1.0)**  |  ✅ Extended language features (Mainly OOP) <br> ✅ Unit test coverage <br> ✅ Refined REPL experience | 🔄 Make CLOX a static-typed language. <br> 🔄 Compile Typed-CLOX to bytecode <br> 🔄 Virtual machine for the bytecode <br> ❌ LOX standard library |

✅ Supported | 🔄 In progress | ❌ In plan  


## Major Changes from Original LOX  

To extend the language and equip it with type system, several changes in grammar are made.

### Type System  

#### Vision of the Type System

|Feature|Status|
|-------|------|
|Primitive types like `integer`|✅|
|Class types|✅|
|Basic type checking|✅|
|Union type and nullable type|🔄|
|Checks for nullability|❌|  

#### Changes from Original LOX

**Variables** should be either declared with a type, or initialized by a type-deducible initializer:
```
var val1:integer; 
var val2=1+1;
```
**Functions** and **Methods** should be either declared with a type, or simple enough for return type to be deduced. All parameters, unlike variables, should be declared with a type.  
A common situation where return type is impossible to be deduced is that a recursive call appear before any `return` statement.
```
fun hello() // type deduced: void
{
   print "hello";
}

fun hello2(a:integer) // type deduced: integer
{
   return a+2*a;
}

fun fib(a:integer)  // type deduced: integer
{
  if(a==1 or a==2)
  {
    return 1;
  }
  
  return fib(a-1)+fib(a-2);
}

fun too_complex(d:integer) // ERROR: too complex for return type deducing
{
   if(d==10)
   {
      print "fin";
   }
   too_complex(d+1);
}

```

### Class  
Now, methods, operator methods and variables can appear in class declaration. Unlike original LOX language, a new keyword, `constructor` is introduced to define a constructor. Furthermore, keyword `operator` is introduced to define operator methods. Note that not every operator can be defined by it.  
Another key different is that `fun` keyword is now required for methods.  

```
class Person {
  var name:string;

  constructor(n:string) {
    this.name=n;
  }

  constructor(n1:string,n2:string) {
    this.name=n1+" "+n2;
  }

  fun sayName() {
    print this.name;
  }

  fun sayName(attach:string) {
    var msg=this.name+" "+attach;
    print msg;
  }

  operator==(another:Person):boolean {
    return this.name==another.name;
  }

}
```

## Documentation

Here is a hand-on quick instruction. Refer to [Documentation for extended LOX language (not exist yet)]() for details.

### Variables and Constants
```
var a=10; // it's a variable
a=20; // variable can be reassigned
```

### Built-in `print` Statement
```
var a=10;
print a; // 10
print a=20; // 20, a's value will be 20 afterwards.
```

// TODO  

## Contributing  

Contributions are what make the open source community such an amazing place to be learned, inspire, and create. Any contributions you make are **greatly appreciated**.  

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)  
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)  
4. Push to the Branch (`git push origin feature/AmazingFeature`)  
5. Open a Pull Request  

Furthermore, you are welcomed to:  

1. [Ask a question](https://github.com/SmartPolarBear/clox/discussions/categories/q-a)   
   Also, have a look at our [FAQs]().  
2. [Start a discussion](https://github.com/SmartPolarBear/clox/discussions/categories/general)    
   Discussions can be about any topics or ideas related to CLOX.  
3. [Make a feature proposal](https://github.com/SmartPolarBear/clox/issues)   
   Language features do you want to appear or not to appear in CLOX? For example, you can propose a new grammar making the lox better, or an idea for library features.   
    
## License

Copyright (c) 2021 SmartPolarBear

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.