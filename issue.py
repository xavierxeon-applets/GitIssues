#!/usr/bin/env python3

import sys

from lib import Session, SessionGitHub, SessionGitLab, Entry

def parseArgs():

   title = ' '.join(sys.argv[1 : ])
   body = str()

   return (title, body)

def main():

   try:
      if Session.isGitHubRepo():
         session = SessionGitHub()
      else:
         session = SessionGitLab()
   except ValueError:
      sys.exit(1)

   if len(sys.argv) < 2: # print open issues
      for entry in session.iterOpen():
         print(entry)
      return

   # add new issue
   title, body = parseArgs()      
   entry = Entry(title, body)
   session.create(entry)
   print(entry)

if __name__ == '__main__':
   main()
