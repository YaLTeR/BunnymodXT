#!/usr/bin/env python3
import json
import struct
from sys import stdin, stdout


# Usage: ./taslogger-to-gt.py < taslogger.log > gt
# Parses the taslogger log file and dumps binary GT data.
def main():
    log = json.load(stdin)

    for physics_frame in log['pf']:
        for command_frame in physics_frame['cf']:
            state = command_frame['postpm']
            stdout.buffer.write(struct.pack('6f', *state['pos'],
                                            *state['vel']))


if __name__ == "__main__":
    main()
