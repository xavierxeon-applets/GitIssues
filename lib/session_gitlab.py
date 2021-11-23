#!/usr/bin/env python3

import sys, json

from xxpystuff.tools import Console

from .session import Session

try:
   import gitlab
except ModuleNotFoundError:
   print('pip3 install --user python-gitlab')
   sys.exit(1)

class SessionGitLab(Session):

   def __init__(self):

      Session.__init__(self)

      url = 'https://' + self._repoUrl
      self._gitlab = gitlab.Gitlab(url, private_token=self._password)

      self._project = None
      for project in self._gitlab.projects.list(all = True):
         if project.http_url_to_repo == self._giturl:
            self._project = project
            break


   def iterOpen(self):

      for issue in self._project.issues.list(all = True, state = 'opened'):
         #print(issue._attrs)
         yield issue.title

      return None

   def create(self, entry):

      if not self._project:
         return

      content = {
         'title': entry.title
         , 'body': entry.body if entry.body else str()
      }

      issue = self._project.issues.create(content)
      entry.number = issue.iid
      