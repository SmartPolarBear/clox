# CLOX
Interpreter and compiler implemented in modern C++ for the lox programing language described in [Crafting Interpreters](https://craftinginterpreters.com/) with extensions in grammars and other features. 

In a long run, this project will support compile to binary and JIT with LLVM.  

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
- [google/googletest](https://github.com/google/googletest)  

## Usage  
`clox` to enter REPL mode.    
`clox --file <source file>` to run a file.  

## Roadmap  

|Naive CLOX|Typed CLOX|CLOXc
|----------|----------|---------|
✅ Basic language features<br><br> **Release: [First preview version of clox interpreter](https://github.com/SmartPolarBear/clox/releases/tag/v0.1.0)**  |  ✅ Extended language features (Mainly OOP) <br> 🔄 Full test coverage <br> 🔄 Refined REPL experience <br> ❌ Introduce the LOX standard library | ❌ Make CLOX a static-typed language. A dynamic subset of lox will be available as well. <br> ❌ Compile Typed-CLOX to binary executable with LLVM <br> ❌ Complete implementation of LOX standard library |

✅ Supported | 🔄 In progress | ❌ In plan  

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