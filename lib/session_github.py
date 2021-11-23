#!/usr/bin/env python3

import json, requests

from xxpystuff.tools import Console

from .session import Session
from .entry import Entry

class SessionGitHub(Session):

   def __init__(self):

      Session.__init__(self)

      self._session = requests.Session()
      self._session.auth = (self._userName, self._password)
      self._url = f'https://api.github.com/repos/{self._repoOwner}/{self._repoName}/issues'

   def iterOpen(self):

      request = self._session.get(self._url)

      if request.status_code != 200:
         print(Console.magenta(str(request.status_code)))
         return None

      data = json.loads(request.content)
   
      for content in data:
         
         if content['state'] != 'open':
            continue

         entry = Entry(content['title'], content['body'], content['number'])
         yield entry

      return None
      
   def create(self, entry):

      content = {
         'title': entry.title
         , 'body': entry.body if entry.body else str()
      }

      request = self._session.post(self._url, json.dumps(content))
      if request.status_code != 201:
         print(Console.magenta(str(request.status_code)))
         return None

      data = json.loads(request.content)                
      entry.number = data['number']      