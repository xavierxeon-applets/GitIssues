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
        'x86_64': 'gcc_64'
    }
}

if not platform.system() in pathDict:
    sys.exit(1)

archDict = pathDict[platform.system()]

if not platform.processor() in archDict:
    sys.exit(1)

path = archDict[platform.processor()]
qtPath += path + '/'

print(qtPath)
