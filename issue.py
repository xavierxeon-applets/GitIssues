#!/usr/bin/env python3

import sys

from lib import Session, Entry

def parseArgs():

   title = ' '.join(sys.argv[1 : ])
   body = str()

   return (title, body)

def main():

   session = Session()

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
