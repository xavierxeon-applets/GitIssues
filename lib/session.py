#!/usr/bin/env python3

import sys, os, requests, json
from pathlib import Path

from xxpystuff.tools import Process, Console

from .entry import Entry

class Session():

   def __init__(self):

      repoUrl, repoOwner, self.repoName = self._readGitCOnfig()
      if not repoUrl or not repoOwner or not self.repoName:
         sys.exit(1)
      
      self.isGitHub = 'github.com' == repoUrl

      userName, self.password = self._readGitCredentials(repoUrl)
      if not userName or not self.password:
         sys.exit(1)

      #print(repoUrl, repoOwner, repoName, userName, password)

      self.session = requests.Session()

      if self.isGitHub:
         self.session.auth = (userName, self.password)
         self.url = f'https://api.github.com/repos/{repoOwner}/{self.repoName}/issues'
      else:
         print(Console.red('TODO: gitlab oauth2 authentication'))
         # maybe https://github.com/requests/requests-oauthlib
         sys.exit(1)
         self.session.auth = (userName, self.password)
         self.url = f'https://{repoUrl}/repos/api/v4/issues'

   def iterOpen(self):

      if self.isGitHub:
         request = self.session.get(self.url)
      else:
         headers = {'PRIVATE-TOKEN': self.password}
         request = self.session.get(self.url, headers = headers)
         print(self.url, headers)

      if request.status_code != 200:
         print(Console.magenta(str(request.status_code)))
         return None

      data = json.loads(request.content)
      if self.isGitHub:
         for content in data:
            
            if content['state'] != 'open':
               continue

            entry = Entry(content['title'], content['body'], content['number'])
            yield entry
      else:
         print(data)
      return None

   def create(self, entry):

      content = {
         'title': entry.title
         , 'body': entry.body if entry.body else str()
      }

      request = self.session.post(self.url, json.dumps(content))
      if request.status_code != 201:
         print(Console.magenta(str(request.status_code)))
         return None

      data = json.loads(request.content)                
      entry.number = data['number']

   def _readGitCOnfig(self):

      giturl = Process.execute(['git', 'config', '--get', 'remote.origin.url'])
      if not giturl:
         print(Console.red('not a git repository'))
         return (None, None)

      giturl = giturl.decode().strip()
      giturl = giturl.replace('https://', '')
      giturl = giturl.replace('.git', '')

      urlParts = giturl.split('/')
      return urlParts

   def _readGitCredentials(self, repoUrl):

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
            if not line.endswith('@' + repoUrl):
               continue

            line = line.replace('https://', '')
            line = line.replace('@' + repoUrl, '')

            lineParts = line.split(':')
            return lineParts