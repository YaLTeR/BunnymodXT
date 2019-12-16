# Bunnymod XT TAS Tests

## Running

You will need Python 3 and a Steam Half-Life installation. GoldSource Package
will work. HD models must be *disabled*, otherwise the tests won't sync (as HD
models change the NPC collision).

You will also need a script that launches Half-Life with Bunnymod XT injected,
passing the command-line arguments through to Half-Life, and then waiting until
Half-Life quits. For Windows, you can find an example script in
`example-windows-launcher.bat`: it needs `BunnymodXT.dll` and `Injector.exe` in
the same folder as the script itself, and you will need to change the `hl.exe`
path inside the script. For Linux, the usual `runhl.sh` will do.

To run the tests, use `run-tests.py --windows path/to/launcher.bat` or
`run-tests.py --linux path/to/launcher.sh` depending on your OS.

Linux tests sync on Bunnymod XT built with GCC and don't sync on Bunnymod XT
built with Clang.

## Adding new tests

1. Edit the .hltas script you want to make test of:
    - Add `_bxt_norefresh 1;bxt_taslog` in the first frame and
      `bxt_taslog;quit` in the last frame.
    - Make sure there are no other `bxt_taslog` or `_bxt_norefresh` commands in
      the script.
1. Run the script in-game to produce `taslogger.log`.
1. Create a folder for the test. Make sure its name doesn't contain dashes.
1. Put the modified .hltas into the folder as `script.hltas`.
1. `taslogger-to-gt.py < path/to/taslogger.log > your_new_folder/gt`
1. Create `settings.txt` in the folder and fill it in:
    - The first line is `windows` or `linux`. This is the test's OS.
    - The second line is the Half-Life game directory, e.g. `valve` or
      `valve_WON` or `gearbox`.
    - The third line is the map name, e.g. `c1a0`.
1. Run the tests to make sure they work.
