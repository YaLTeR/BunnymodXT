BunnymodXT
==========

Bunnymod XT (BXT for short) is a cross-platform tool that provides speedrunning and TAS-related features for GoldSource games such as Half-Life and its modifications.

##VAC BAN WARNING: Do NOT connect to servers with this injected, or you might get VAC banned!

#Installation
- Windows: [Is described here](https://github.com/YaLTeR/BunnymodXT/wiki).
- Linux: Download / build **libBunnymodXT.so** and launch Half-Life with it via **LD_PRELOAD**. [Here's a helper script](http://tastools.readthedocs.org/en/latest/tastools.html#half-life-execution-script) for launching Half-Life from your terminal.

#Documentation
[On the Wiki pages](https://github.com/YaLTeR/BunnymodXT/wiki).

###Environment variables
- **BXT_SCRIPT** - if set to a filename of a hltas script, loads the non-shared RNG from that script on load.
- **BXT_LOGFILE** - if set, logs all Bunnymod XT messages into a file with that filename.
- **SPTLIB_DEBUG** - if set to 1, logs all dlopen, dlclose and dlsym calls.

#Building
####Windows
- You will need at least Visual Studio 2015 Update 2.
- Get [Boost](http://www.boost.org/).
- Get [RapidJSON](https://github.com/miloyip/rapidjson/releases/tag/v1.0.2).
- Create a build directory along the *src* directory.
- Run `cmake -DBOOST_ROOT=path\to\boost\base\dir -DRapidJSON_ROOT=path\to\rapidjson\base\dir -Wno-dev ..` from the build directory.
- Compile the **ALL_BUILD** project from the generated VS solution.

If you want to make a Windows XP-compatible build, additionally specify `-T v140_xp` (or the equivalent for your Visual Studio version) in the `cmake` command line arguments.

####Linux
- Get Boost.
- Get [RapidJSON](https://github.com/miloyip/rapidjson/releases/tag/v1.0.2).
- Create a build directory along the *src* directory.
- Run `cmake -Wno-dev ..` from the build directory. You might have to specify `-DRapidJSON_ROOT=path/to/rapidjson/base/dir`.
- Run `make` from the build directory.
