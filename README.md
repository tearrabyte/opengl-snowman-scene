<a id="readme-top"></a>

<!-- PROJECT HEADER -->
<div align="center">

  <!-- SHIELDS -->
  [![C Language][clanguage-shield]][clanguage-url]
  [![OpenGL][opengl-shield]][opengl-url]
  [![FreeGLUT][freeglut-shield]][freeglut-url]
  [![Windows][windows-shield]][windows-url]


  <br />

  <!-- TODO: ADD HEADING LOGO HERE -->
  
  # Snowman Scene
  #### *A real-time 2D OpenGL simulation*
  A stylised snowman scene featuring animated snowfall, dynamic day/night cycling, wind simulation, and interactive facial tracking. 

  <!-- DOWNLOAD SHIELD -->
  <a href="https://github.com/tearrabyte/opengl-snowman-scene/releases/latest">
    <img src="https://img.shields.io/badge/⬇_Download-Run_Executable-2ea44f?style=for-the-badge"/>
  </a>
    
</div>

<!-- TABLE OF CONTENTS -->
<br />
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#features">Features</a></li>
    <li>
      <a href="#demo">Demo</a>
    </li>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#run-the-executable-recommended">Run the Executable (Recommended)</a></li>
        <li><a href="#for-developers">For Developers</a></li>
      </ul>
    </li>
    <li><a href="#controls">Controls</a></li>
    <li><a href="#built-with">Built With</a></li>
    <li><a href="#credits">Credits</a></li>
  </ol>
</details>


<!-- ABOUT THE PROJECT -->
## About The Project

A real-time 2D graphics simulation built in C using OpenGL and FreeGLUT. The project renders an animated snowman scene featuring environmental effects and interactive elements.  

The system focuses on real-time procedural animation, including snowfall dynamics, wind influence, and smooth day/night lighting transitions. The snowman includes basic interactive facial tracking to respond to cursor movement, adding a layer of simple interactivity to the scene.  

This project was developed as a graphics programming exercise to explore low-level rendering, animation loops, and real-time input handling using legacy OpenGL.

<br />

<!-- FEATURES -->
## Features
* **Real-time snowfall simulation**  
  Procedurally animated snow particles with layered depth and continuous motion.
  
* **Wind system**  
  Dynamic wind influence affecting snowfall direction and intensity.
  
* **Day/night cycle**  
  Smooth transitions between lighting states to simulate time progression.

* **Interactive snowman tracking**  
  Snowman eyes respond to cursor movement, creating simple interaction.

* **Real-time rendering and animation system (OpenGL)**  
  Frame-based update loop handling scene rendering, motion, and animation.
      
* **Simplified procedural ground/terrain generation**  
  Randomised point-based terrain forming the base of the scene.
    
* **Diagnostics overlay**  
  Optional real-time display of particle count, wind strength, and controls for user feedback and system state visualisation.

* **Input-driven environment controls**  
  Keyboard controls for toggling wind, snowfall, and lighting behaviour.

<p align="right"><a href="#readme-top">Back to top</a></p>


<!-- Demo -->
## Demo

<!-- GIF HERE -->
<p align="center">
  <img src="assets/demo.gif" width="80%" />
</p>

<!-- IMAGE HERE -->
<p align="center">
  <img src="assets/screenshot_day.png" width="26%" />
  <img src="assets/screenshot_night.png" width="26%" />
  <img src="assets/screenshot_wind.png" width="26%" />
</p>

The GIF demonstrates the real-time simulation including snowfall, wind influence, dynamic lighting transitions, and interactive snowman tracking.   

The screenshots show different environmental states: daytime lighting, nighttime scene, and high-wind snowfall conditions.  


<p align="right"><a href="#readme-top">Back to top</a></p>


<!-- GETTING STARTED -->
## Getting Started
This project can be run either as a prebuilt executable (recommended) or built from source for development purposes.

<!-- RUN THE EXECUTABLE -->
### Run The Executable (Recommended)
Download the latest build: 
> https://github.com/tearrabyte/opengl-snowman-scene/releases/latest

1. Download the Windows build (.zip)
2. Extract the folder.
3. Run: `snowman-scene.exe`

No additional setup is required for the executable version.

<p align="right"><a href="#readme-top">Back to top</a></p>

<!-- FOR DEVELOPERS -->
### For Developers

<!-- PREREQUISITES -->
#### Prerequisites
- Visual Studio (C++ Desktop Development workload)
- FreeGLUT library

<!-- INSTALLATION -->
#### Clone Repository
```sh
git clone https://github.com/tearrabyte/opengl-snowman-scene.git
```

#### FreeGLUT Setup
1. Download FreeGLUT.
2. Create the following folder structure in the project root: `external/freeglut`
3. Add the required FreeGLUT files:
- `include/GL` → header files
- `lib` → library files
- `bin` → DLL files

#### Open and Build
1. Open `snowman-scene.slnx` in Visual Studio.
2. Select **Win32 (x86)** configuration
3. Build solution
4. Run in Visual Studio

<p align="right"><a href="#readme-top">Back to top</a></p>


<!-- Controls -->
## Controls
- q - quit / exit
- d - toggle diagnostics overlay
- s - toggle snowfall
- left arrow - apply wind to the left (strength increases while held)
- right arrow - apply wind to the right (strength increases while held)
- c - toggle day/night cycle
<p align="right"><a href="#readme-top">Back to top</a></p>

<!-- BUILT WITH -->
## Built With
#### Language and Graphics
[![C Language][clanguage-shield]][clanguage-url] [![OpenGL][opengl-shield]][opengl-url] [![FreeGLUT][freeglut-shield]][freeglut-url]

#### Development Tools
[![Visual Studio][visualstudio-shield]][visualstudio-url]

<p align="right"><a href="#readme-top">Back to top</a></p>

<!-- CREDITS -->
## Credits
*Developed as part of an independent academic graphics programming assignment.*   
[![@tearrabyte][tearrabyte-shield]][tearrabyte-url]  

<p align="right"><a href="#readme-top">Back to top</a></p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- PROJECT BUILT WITH SHIELDS-->
[clanguage-shield]: https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c&logoColor=white
[clanguage-url]: https://en.wikipedia.org/wiki/C_(programming_language)

[opengl-shield]: https://img.shields.io/badge/OpenGL-2D-green?style=for-the-badge&logo=opengl&logoColor=white
[opengl-url]: https://opengl.org/

[freeglut-shield]: https://img.shields.io/badge/FreeGLUT-Library-orange?style=for-the-badge
[freeglut-url]: https://freeglut.sourceforge.net/

[windows-shield]: https://img.shields.io/badge/Windows-Platform-lightgrey?style=for-the-badge&logo=windows&logoColor=white
[windows-url]: https://microsoft.com/windows/

[visualstudio-shield]: https://img.shields.io/badge/Visual_Studio_2026-v.11619.145-5C2D91?style=for-the-badge
[visualstudio-url]: https://visualstudio.microsoft.com/

<!-- PROJECT CREDITS SHIELDS -->
[tearrabyte-shield]: https://img.shields.io/badge/GitHub-tearrabyte-181717?style=for-the-badge&logo=github
[tearrabyte-url]: https://github.com/tearrabyte
