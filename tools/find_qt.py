#!/usr/bin/env python3

import os
import platform
import re
from pathlib import Path
import sys

homePath = str(Path.home())
qtPath = homePath + '/Qt/'

for entry in os.scandir(qtPath):

    if not entry.is_dir():
        continue

    if not re.match('\d+\\.\d+\\.\d+', entry.name):
        continue

    qtPath += entry.name + '/'

# python 3.10 use "match"?
pathDict = {
    'Linux': {
        'x86_64': 'gcc_64',
        'armv7l': '.'
    }
}

print(platform.machine())

if not platform.system() in pathDict:
    print('system', platform.system())
    sys.exit(1)

archDict = pathDict[platform.system()]

if not platform.machine() in archDict:
    print('machine',platform.machine())
    sys.exit(1)

path = archDict[platform.machine()]
qtPath += path + '/'

print('path', qtPath)
