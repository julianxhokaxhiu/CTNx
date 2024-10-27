![License](https://img.shields.io/github/license/julianxhokaxhiu/CTNx) ![Overall Downloads](https://img.shields.io/github/downloads/julianxhokaxhiu/CTNx/total?label=Overall%20Downloads) ![Latest Stable Downloads](https://img.shields.io/github/downloads/julianxhokaxhiu/CTNx/latest/total?label=Latest%20Stable%20Downloads&sort=semver) ![Latest Canary Downloads](https://img.shields.io/github/downloads/julianxhokaxhiu/CTNx/canary/total?label=Latest%20Canary%20Downloads) ![GitHub Actions Workflow Status](https://github.com/julianxhokaxhiu/CTNx/actions/workflows/main-0.0.1.yml/badge.svg?branch=master)

<div align="center">
  <img src="https://github.com/julianxhokaxhiu/CTNx/blob/master/.logo/logo_nobg.png" alt="">
  <br><strong><small>Temporary logo until final one is ready. Made with DALL-E 3</small></strong>
</div>

# CTNx

Next generation modding platform for Chrono Trigger ( native Steam 2018 release support! )

## Introduction

CTNx is a new modding platform for Chrono Trigger. Inspired by [FFNx](https://github.com/julianxhokaxhiu/FFNx) aims at bringing the same set of functionalities in time.

CTNx today in a nutshell:

- Uses an easy, drag-n-drop installation experience, see [How to Install](docs/how_to_install.md)

## The Team

CTNx is developed by a core team, currently composed of:

- [Julian Xhokaxhiu](https://github.com/julianxhokaxhiu) (TrueOdin, 🇪🇸 Spain)

We are always open for contributions via PRs, and in case you want to join the core team, feel free to approach us on Discord and we will evaluate on a case-by-case basis.

## Features

### As a user

- [/LARGEADDRESSAWARE](https://docs.microsoft.com/en-us/cpp/build/reference/largeaddressaware-handle-large-addresses?view=vs-2019) support. Up to 3.5GB of RAM available for mods (this requires the [4GB Patch](https://ntcore.com/?page_id=371) in your Chrono Trigger.exe).
- Native speedhack support (coming soon)
- Ambient effects layer (coming soon)
- Voice acting layer.

### As a modder

- Support for [Hext](https://forums.qhimm.com/index.php?topic=13574.0) patching files inside of the [hext_patching_path](misc/CTNx.toml#L113)

## Documentation

For a more in-depth documentation feel free to visit the [docs/](docs/) folder.

## Tech Stack

If you're curious to know, CTNx makes use of:

- C++ code base
- Latest MSVC available on [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [vcpkg](https://vcpkg.io/) (dependency manager)
- [CMake](https://cmake.org/) (make files)
- [FFMpeg](https://www.ffmpeg.org/) with H/W accelleration support
- [VGMStream](https://github.com/losnoco/vgmstream) using FFMpeg as backend (with loop support!)
- [tomlplusplus](https://github.com/marzer/tomlplusplus) (configuration management)
- [StackWalker](https://github.com/JochenKalmbach/StackWalker) (log file stack traces)
- [SoLoud](https://github.com/jarikomppa/soloud) (audio engine used to playback audio, music or voice files)
- [mimalloc](https://github.com/microsoft/mimalloc) (a compact general purpose allocator with excellent performance)
- [plog](https://github.com/SergiusTheBest/plog) (logging library)
- [Detours](https://github.com/microsoft/Detours) (function replacement library)
- [Cocos2d-x](https://github.com/cocos2d/cocos2d-x) (game framework)

## How to build

Available build profiles:

- x86-Release (default, the same used to release artifacts in this Github page)
- x86-RelWithDebInfo (used while developing to better debug some issues)
- x86-MinSizeRel
- x86-Debug (prefer it if you want to use a debugger attached to the game)

Once the project is built you can find the output in this path: `.build/bin`

### Preparation

> **Please note:**
>
> CTNx uses vcpkg as a package manager to resolve dependencies. Failing to follow these steps will result in build errors.

0. Clone the [vcpkg](https://vcpkg.io) project in the root folder of your `C:` drive (`git clone https://github.com/Microsoft/vcpkg.git`)
1. Go inside the `C:\vcpkg` folder and double click `bootstrap-vcpkg.bat`
2. Open a `cmd` window in `C:\vcpkg` and run the following command: `vcpkg integrate install`

### NuGet

> **Please note:**
>
> This step will speed up your compilation times by avoiding the vcpkg dependencies rebuild.

0. Make sure you have [NuGet CLI installed](https://learn.microsoft.com/en-us/nuget/install-nuget-client-tools?tabs=windows#install-nugetexe).
1. [Create a Personal Access token ( classic )](https://github.com/settings/tokens/new) with the `write:packages` permission.
2. Open a `cmd` window and run the following commands ( replace `YOUR_GITHUB_USERNAME` and `YOUR_GITHUB_PAT` accordingly ):
```pwsh
$ nuget sources add -Name github -Source "https://nuget.pkg.github.com/tsunamods-codes/index.json" -Username YOUR_GITHUB_USERNAME -Password YOUR_GITHUB_PAT -StorePasswordInClearText
$ nuget setApiKey YOUR_GITHUB_PAT -Source "https://nuget.pkg.github.com/tsunamods-codes/index.json"
```

### Visual Studio

> **Please note:**
>
> By default Visual Studio will pick the **x86-Release** build configuration, but you can choose any other profile available.

0. Download the the latest [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) installer
1. Run the installer and import this [.vsconfig](.vsconfig) file in the installer to pick the components required to build this project
2. Make sure you select the English Language pack in the language list before clicking Install
3. Once installed, open this repository **as a folder** in Visual Studio
4. Choose as preset in the status bar the one you desire
5. Click the `Build` button

### Visual Studio Code

0. **REQUIRED!** Follow up the steps to install Visual Studio, which will also install the MSVC toolchain
1. Download and install the latest [Visual Studio Code](https://code.visualstudio.com/) release
2. Install the following extensions:
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
3. Open this repository as a folder in Visual Studio Code
4. Choose as preset in the status bar the one you desire
5. Click the button on the status bar `Build`

### Neovim with clangd (optional)

0. **REQUIRED!** Follow up the steps to install Visual Studio, which will also install the MSVC toolchain
1. **REQUIRED!** Have [Neovim](https://neovim.io/) installed with [clangd](https://clangd.llvm.org/) as LSP
2. Install the extension `Clang Power Tools` in [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
3. Open the solution `CTNx.sln` under the `.build` directory with **Visual Studio Community** (**sln** file is generated by the build process)
4. Right-click on **CTNx** solution (under the **Solution Explorer**) then click on **Clang Power Tools** -> **Export Compilation Database**
5. Copy the `compile_commands.json` generated file into the root of the repository
6. Open **Neovim** and enjoy!

To build from the terminal (example with *RelWithDebInfo*):
- For dependency use: `cmake --preset RelWithDebInfo`
- For building the project: `cmake --build --preset RelWithDebInfo`

**NOTICE**: For the dependency step, make sure that PATH variable does not contain any UNIX command tools since vcpkg build process is based on UNIX tools.
Also on terminal it might not auto detect the kernel32_lib, so pass it with `-DCMAKE_LIBRARY_PATH=%kernel32_lib%`
(e.g. `cmake --preset RelWithDebInfo -DCMAKE_LIBRARY_PATH="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x86"`)

## Auto-Formatting

### CMake Files

0. **REQUIRED!** Install [Python](https://www.python.org/)
1. Install [cmake-format](https://github.com/cheshirekow/cmake_format#installation) and make sure the binary is available in your PATH environment variable
2. **OPTIONAL!** Integrate it [in your own IDE](https://github.com/cheshirekow/cmake_format#integrations) (eg. for Visual Studio Code use [the relative extension](https://marketplace.visualstudio.com/items?itemName=cheshirekow.cmake-format))

## Support

CTNx offers multiple support channels, pick the one you prefer

### Discord

- Tsunamods CTNx: https://discord.gg/Urq67Uz (remember to hit the relative reaction!)

### Github

- Issues: https://github.com/julianxhokaxhiu/CTNx/issues

## Credits

This project could have not been a reality if those people would have not worked on Chrono Trigger with their deep passion and knowledge.
CTNx makes use also of their work, and I will never be enough grateful to those people. The order is purely Alphabetical.

These people are:

- [Biggs McWedge ](https://github.com/TheRealBiggs)
  - for various discoveries in the game engine code
  - for the initial voice acting implementation and research
- [Maciej Trębacz ](https://github.com/maciej-trebacz)
  - for various discoveries in the game engine code

## License

CTNx is released under GPLv3 license. You can get a copy of the license here: [COPYING.txt](COPYING.txt)

If you paid for CTNx, remember to ask for a refund from the person who sold you a copy. Also make sure you get a copy of the source code (if it was provided as binary only).

If the person who gave you this copy refuses to give you the source code, report it here: https://www.gnu.org/licenses/gpl-violation.html

All rights belong to their respective owners.
