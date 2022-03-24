#!/usr/bin/env python3

from urllib.parse import urlparse

from xxpystuff.tools import Process

from subprocess import Popen, PIPE


def main():
    gitProcess1 = Process('git')
    gitProcess1.start(['config', '--get', 'remote.origin.url'])

    url = gitProcess1.output.strip()
    if not url:
        return

    print(url)
    url = urlparse(url)

    data = f'protocol={url.scheme}\n'
    data += f'host={url.hostname}\n'
    data += f'path={url.path[1:]}\n'
    if(url.username):
        data += f'username={url.username}\n'
    data += '\n'

    with Popen(['git', 'credential', 'fill'], stdout=PIPE, stderr=PIPE, stdin=PIPE, bufsize=1, universal_newlines=True) as process:

        output, error = process.communicate(data)

        print(output.encode())
        print(error.encode())


if __name__ == '__main__':
    main()
