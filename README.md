
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/N0822325/FYP">
    <img src="Resources/FYP.png" alt="Logo" width="120" height="80">
  </a>

  <h3 align="center">Graphics Engine for Game Development</h3>

  <p align="center">
    A Lightweight and Easy to use, OpenGL Powered Graphics Framework
    <br />
    <a href="https://google.com"><strong>Explore the Documentation »</strong></a>
    <br />
    <br />
    <a href="https://github.com/N0822325/FYP/issues">Report Bug</a>
    ·
    <a href="https://github.com/N0822325/FYP/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
  * [Built With](#building-the-project)
* [Usage](#usage)
* [Roadmap](#roadmap)
* [Contributing](#contributing)
* [License](#license)
* [Contact](#contact)
* [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

This is my Final Year Project as a student of Nottingham Trent University. I chose this project to gain experience in the X11 and Win32 apis, and OpenGL development.

The purpose of this project is to create a lightweight, easy to use graphics framework with 2D and 3D functionality for c++ game development.

_This Project is currently in the primary stages of development_



<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

There are no prerequisites right now :)


### Installation

Clone the repo
```sh
git clone https://github.com/N0822325/FYP
```


<!-- USAGE EXAMPLES -->
## Examples
See these examples for how to structure your project
1. (2D) Pong    - https://github.com/N0822325/FYP-Pong
2. (3D) Sandbox - https://github.com/N0822325/FYP-Sandbox
## Usage
1. Create your cpp file
2. Include `Engine.h`
3. Make a new Class and Inherit 'Class Engine'
4. Call [construct()](https://example.com) and [start()](https://example.com)
5. override virtual function [userUpdate()](https://example.com)

_For more Functionality, Please Refer to the [Documentation](https://example.com)_


### Building the Project
[Linux]
* Make sure the mesa-utils package is installed

Using the GCC Compiler
```sh
gcc [Program.cpp] -lX11 -lGL -lGLU -lpthread
```

[Windows]

Visual Studio
* Create a new project
* Add header files to the project, and create / add example main program file
* Disable Unicode by right clicking project -> Properties -> Advanced -> Character Set, and change it to Multi-Byte
* Run the main program to compile

VS Code / MINGW Compiler
* Download and install the mingw compiler
* Add mingw to the windows PATH environment by searching "Edit environment variables" -> Click Path then Edit -> Click New then add the path to mingws' bin folder (default is C:\Program Files\mingw-w64\ . . .\mingw64\bin)
* [VSC] Install C/C++ Extension for VS Code

Compile in Terminal
* Follow previous steps in installing mingw then execute the following to compile program from terminal
```sh
gcc [Program.cpp] -lstdc++ -lopengl32
```


<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/N0822325/FYP/issues) for a list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

Want to add some much needed functionality? Then follow these steps and I'll merge it into the main branch :smile:

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.md` for more information.



<!-- CONTACT -->
## Contact

Bradley Smith - bradleysmith20000@gmail.com - N0822325@my.ntu.ac.uk - [LinkedIn](https://linkedin.com/in/bradley-smith-9b82641bb)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [Readme Template](https://github.com/othneildrew/Best-README-Template)




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/N0822325/FYP.svg?style=flat-square
[contributors-url]: https://github.com/N0822325/FYP/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/N0822325/FYP.svg?style=flat-square
[forks-url]: https://github.com/N0822325/FYP/network/members
[stars-shield]: https://img.shields.io/github/stars/N0822325/FYP.svg?style=flat-square
[stars-url]: https://github.com/N0822325/FYP/stargazers
[issues-shield]: https://img.shields.io/github/issues/N0822325/FYP.svg?style=flat-square
[issues-url]: https://github.com/N0822325/FYP/issues
[license-shield]: https://img.shields.io/github/license/N0822325/FYP.svg?style=flat-square
[license-url]: LICENSE.md
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/bradley-smith-9b82641bb
[product-screenshot]: images/screenshot.png
