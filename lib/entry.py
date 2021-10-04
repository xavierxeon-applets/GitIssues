#!/usr/bin/env python3

class Entry:

   def __init__(self, title, body = None, number = None):

      self.title = title
      self.body = body
      self.number = number

   def __str__(self):

      text = self.title
      if self.body:
         text = text + ' [' + self.body + ']'
      if self.number:
         text = text + ' @ ' + str(self.number)
      
      return text