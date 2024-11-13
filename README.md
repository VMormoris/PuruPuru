# Installation

If you want to try out the tool just go [here](https://github.com/VMormoris/PuruPuru/releases) download the latest release version and use it.

If you want to expand the tool for your own game:
- Clone the repo:
    ```git clone --recursive https://github.com/VMormoris/PuruPuru```
- Create project:
  - In **Windows** run `premake5 vs20**`
  - In **Linux** run `premake5 gmake2`
- Build:
  - In **Windows** open `PuruPuru.sln` with your favorite editor (or use `msbuild`)
  - In **Linux** run `make`

_For more information on how to use premake5 can be found in their [website](https://premake.github.io/docs/)_

# Third Party Libraries

  * _[entt](https://github.com/skypjack/entt) As an entity-component-system._
  * _[imgui-node-editor](https://github.com/thedmd/imgui-node-editor) Node editor._
  * _[yaml-cpp](https://github.com/jbeder/yaml-cpp) Is used for (de)serialization._

# Contributing
  * _For reporting a bug please open an issue describing the bug as well a way to recreate it._
  * _If you want to contribute or suggest a new feature open an issue describing what you want._
  * _Feel free to work to any of the opened issues for a bug and make a pull request._
