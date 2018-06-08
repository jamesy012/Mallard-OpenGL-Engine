# Mallard OpenGL Engine

An OpenGL game engine written in C++.

I would not recommend at this stage to use this engine to build a game.

It could still be used to learn more about engine programming and how I delt with some parts such as shader uniforms.

I am still looking to improve the engine add more features, 
that I have in a trello, it has a huge list of features I would like to add or attempt at some point. 

Feel free to point out any parts which could be improved.

I had an Android port made, It had everything execpt file loading due to me not understanding how to build assimp dll's for Android.
(If anyone could help me there, it would be much appreciated)

## Features 

* Shader Handler
  * Uniform system
    * Easliy edit uniforms and apply them to the shader
    * Local storage of uniform data
  * Add preprocessor Define's into the shader code
  * Include other files when building shaders
    * Such as lighting code or common data between shaders.
* Framebuffer system
  * Add only what you need to the Framebuffer
  * Stores the last frame that was rendered to be used 
* Resource manager
  * Keeps a copy of each loaded file in ram and when that file is loaded again it takes that copy
* Texture
  * Can read and write pixels
  * Can pack textures into a larger texture

## How to build

* Pull or download from this repo
* Make TestProject the Startup project
* (You might need to change the Working directory of TestProject And MallardEngine to '$(SolutionDir)_WorkingDir')
* Good to go.

## Issues

* Wont build in 64 bit mode due to a libary - just need to build it for x64
* If you get errors when building it could be from Robocopy,
You will just have to copy the dlls from the Dependencies folder into _WorkingDir

## Why am I making it?

Mainly to learn more about engine programming and to improve my OpenGL skills.

I made an Opengl during my course at AIE [The source for my last version.](https://bitbucket.org/jamesy012/aieopengl/src/bc2dfbbee3f4cd8182576028425c625dcc509b31/Projects/LibEngine/?at=master)

## Libarys used

* [Assimp - Model Loading](https://github.com/assimp/assimp)
* [STB - Image Loading and Font Loading/Glyphs](https://github.com/nothings/stb)
* [GLFW - Window and Input](http://www.glfw.org/)
* [Glm - 3D Math](https://glm.g-truc.net/0.9.8/index.html)
* [GLEW - Opengl Wrapper](http://glew.sourceforge.net/)
* [Bullet - Physics](https://github.com/bulletphysics/bullet3)
