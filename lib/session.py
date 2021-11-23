#!/usr/bin/env python3

import sys, os
from pathlib import Path

from xxpystuff.tools import Process, Console

class Session():

   def __init__(self):

      self._repoUrl, self._repoOwner, self._repoName = self._readGitCOnfig()
      if not self._repoUrl or not self._repoOwner or not self._repoName:
         sys.exit(1)
      
      self._userName, self._password = self._readGitCredentials()
      if not self._userName or not self._password:
         sys.exit(1)

   @staticmethod
   def isGitHubRepo():

      giturl = Process.execute(['git', 'config', '--get', 'remote.origin.url'])
      if not giturl:
         print(Console.red('not a git repository'))
         raise ValueError

      giturl = giturl.decode().strip()      
      return 'github.com' in giturl

   def iterOpen(self):

      raise NotImplementedError

   def create(self, entry):

      raise NotImplementedError

   def _readGitCOnfig(self):

      self._giturl = Process.execute(['git', 'config', '--get', 'remote.origin.url'])
      if not self._giturl:
         return (None, None, None)

      self._giturl = self._giturl.decode().strip()

      tmp_giturl = self._giturl.replace('https://', '')
      tmp_giturl = tmp_giturl.replace('.git', '')

      urlParts = tmp_giturl.split('/')
      return urlParts

   def _readGitCredentials(self):

      gitCredentialsFileName = str(Path.home()) + '/.git-credentials'
      if not os.path.exists(gitCredentialsFileName):
         gitCredentialsFileName = str(Path.home()) + '/.git-credentials-wsl'
         if not os.path.exists(gitCredentialsFileName):
            print(Console.red('can not find git credentials'))
            print(gitCredentialsFileName)
            return (None, None)

      with open(gitCredentialsFileName) as infile:
         for line in infile:
            line = line.strip()
            if not line.endswith('@' + self._repoUrl):
               continue

            line = line.replace('https://', '')
            line = line.replace('@' + self._repoUrl, '')

            lineParts = line.split(':')
            return lineParts