

# Devilution
Diabled: Understanding magic behind the greatest crpg ever

THis requires an original copy of `diabdat.mpq`. None of the Diablo 1 game assets are provided by this project. To get a legitimate copy of the game assets purchase from gog.com or battle.net

# Introduction
I copied this from devilution because i wanted the source code to use for my attempt at rebuilding in unreal 

# Purpose
informational, I plan on using this to guide my work in unreal. 

# Compiling
Diablo was developed on Windows 95 using Visual C++ 4.20 and later 5.10 and 6 for newer patches.

### Building with Visual Studio 2010-2017
- Open the project solution `Diablo.sln`, choose `Debug` or `Release`, and then `Build Solution`.

Make sure to disable Data Execution Prevention. `Storm.dll` uses dynamic compilation to improve rendering performance but fails to mark the resulting memory page as executable, leading to a protection fault when trying to draw.
- Configuration options -> Linker -> Advanced -> Data Execution Prevention (DEP).
- Set this value to: No (/NXCOMPAT: NO).

You will also need the following dependencies installed if you are using Visual Studio 2017.
Make sure to enable these when installing (or modify your installation):
- Requires "Windows 8.1 SDK" (Target Platform)
- Requires "Visual C++ MFC for x86 and x64" (For afxres.h)
- Requires "Windows Universal CRT SDK" (For ctype.h)








# Legal
idk im just having fun with this
