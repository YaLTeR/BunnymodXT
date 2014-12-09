BunnymodXT
==========

The new Bunnymod XT!

This is the Bunnymod for Half-Life mods. Its primary function is to remove the bhop cap, it also can do other stuff like legit autojump.

##VAC BAN WARNING: Do NOT connect to servers with this injected, or you might get VAC banned!

#Installation
- Windows: [Is described here.](https://github.com/YaLTeR/BunnymodXT-Injector)
- Linux: Download / build **libBunnymodXT.so** and launch Half-Life with it via **LD_PRELOAD**. [Here's a helper script](http://tastools.readthedocs.org/en/latest/tastools.html#half-life-execution-script) for launching Half-Life from your terminal.

#CVars
- **y_bxt_bhopcap** - if 0, there's no bhop cap, if 1, there's one. As simple as that.
- **y_bxt_bhopcap_prediction** - same thing, but clientside. Should generally be set to the same as *y_bxt_bhopcap*. This especially matters when you're playing on servers - set accordingly to the presence of the bhop cap on the server in order to eliminate lag.
- **y_bxt_autojump** - if 1, you jump automatically when holding *+jump*. This has the required consequent frame jump checks, so is **legit**.
- **y_bxt_autojump_prediction** - a clientside thing again, this time more important as it handles the stepsounds.

###Environment variables (Linux)
- **BXT_LOGFILE** - if set, logs all Bunnymod XT messages into a file with that filename.
- **SPTLIB_DEBUG** - if set to 1, logs all dlopen, dlclose and dlsym calls.

#Building
- Windows: You will need [Detours Express 3.0](http://research.microsoft.com/en-us/downloads/d36340fb-4d3c-4ddd-bf5b-1db25d03713d/default.aspx) and Visual Studio 2013 or later (express version will do).
- Linux: Clone this repository and execute **cmake .** and **make** in its directory.
