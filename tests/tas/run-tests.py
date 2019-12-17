#!/usr/bin/env python3

from argparse import ArgumentParser
from itertools import zip_longest
import json
import math
from os import listdir, remove, rename
from os.path import join, isdir, isfile, realpath, dirname
import struct
from subprocess import run
from sys import exit


def read_gt(path):
    with open(path, 'rb') as f:
        for data in iter(lambda: f.read(24), b''):
            values = struct.unpack('6f', data)
            yield values[:3], values[3:]


def read_log(path):
    with open(path, 'r') as f:
        log = json.load(f)

    for physics_frame in log['pf']:
        for command_frame in physics_frame['cf']:
            state = command_frame['postpm']
            yield state['pos'], state['vel']


# HL seems to stop command-line arguments at a ':'.
# This function replaces the colon after the drive on Windows.
def remove_windows_drive_colon(path):
    # If path looks like C:\something, make it \\127.0.0.1\C$\something
    if path[1] == ':':
        path = '\\\\127.0.0.1\\' + path[0] + '$' + path[2:]

    return path


def main():
    parser = ArgumentParser()
    parser.add_argument(
        '--windows',
        '-w',
        help=("script that launches Windows Half-Life with Bunnymod XT "
              "and waits for it to exit"))
    parser.add_argument(
        '--linux',
        '-l',
        help=("script that launches Linux Half-Life with Bunnymod XT "
              "and waits for it to exit"))
    args = parser.parse_args()

    if args.windows is None and args.linux is None:
        print("Please specify either --linux or --windows (or both).")
        exit(1)

    script_dir = dirname(realpath(__file__))
    script_dir = remove_windows_drive_colon(script_dir)
    if '-' in script_dir or ':' in script_dir:
        print(("The script directory '%s' contains a dash '-' or a colon ':'. "
               "Unfortunately, that's not supported due to how Half-Life's "
               "command-line argument parsing works. Please copy the entire "
               "test folder into a directory without dashes or colons.")
              % script_dir)
        exit(1)

    taslogger_path = join(script_dir, 'taslogger.log')

    some_test_failed = False
    for test_dir_name in listdir(script_dir):
        test_dir = join(script_dir, test_dir_name)
        if not isdir(test_dir):
            continue

        print('Test:', test_dir)

        with open(join(test_dir, 'settings.txt'), 'r') as f:
            os = f.readline().strip()
            game_dir = f.readline().strip()
            map_name = f.readline().strip()

        if os == 'windows':
            if args.windows is None:
                print('Skipping because Windows launcher was not specified')
                continue
            launcher = args.windows
        elif os == 'linux':
            if args.linux is None:
                print('Skipping because Linux launcher was not specified')
                continue
            launcher = args.linux
        else:
            print('Unknown OS:', os)
            exit(1)

        script_path = join(test_dir, 'script.hltas')

        if isfile(taslogger_path):
            remove(taslogger_path)

        taslogger_save_name = test_dir_name + '_fail_taslogger.log'
        taslogger_save_path = join(script_dir, taslogger_save_name)
        if isfile(taslogger_save_path):
            remove(taslogger_save_path)

        print('Running the script')

        run([
            launcher, '-game', game_dir, '+bxt_taslog_filename',
            taslogger_path, '+map', map_name, '+bxt_tas_loadscript',
            script_path
        ])

        print('Processing the log')

        gt_path = join(test_dir, 'gt')
        gt = read_gt(gt_path)
        log = read_log(taslogger_path)

        fail = False
        extra_gt_frames = 0
        extra_log_frames = 0
        sentinel = object()
        for frame, (gt_value, value) in enumerate(
                zip_longest(gt, log, fillvalue=sentinel)):
            if gt_value is sentinel:
                extra_log_frames += 1
                continue
            elif value is sentinel:
                extra_gt_frames += 1
                continue

            gt_pos, gt_vel = gt_value
            pos, vel = value

            if not all(math.isclose(a, b) for a, b in zip(gt_pos, pos)):
                print('Position mismatch at frame %d:' % frame)
                print('\tExpected: %14.6f %14.6f %14.6f' % tuple(gt_pos))
                print('\t     Got: %14.6f %14.6f %14.6f' % tuple(pos))
                fail = True
                break

            if not all(math.isclose(a, b) for a, b in zip(gt_vel, vel)):
                print('Velocity mismatch at frame %d:' % frame)
                print('\tExpected: %14.6f %14.6f %14.6f' % tuple(gt_vel))
                print('\t     Got: %14.6f %14.6f %14.6f' % tuple(vel))
                fail = True
                break

        if not fail and (extra_gt_frames != 0 or extra_log_frames != 0):
            print('Frame count mismatch: expected %d, got %d' %
                  (frame - extra_log_frames + 1, frame - extra_gt_frames + 1))
            fail = True

        if fail:
            print('Test failed; saving taslogger.log to', taslogger_save_name)
            rename(taslogger_path, taslogger_save_path)
            some_test_failed = True
        else:
            print('Test passed')

    if isfile(taslogger_path):
        remove(taslogger_path)

    if some_test_failed:
        exit(1)


if __name__ == "__main__":
    main()
