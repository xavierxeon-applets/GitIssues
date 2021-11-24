#include "Exception.h"

Exception::Exception(const Cause& cause)
   : QException()
   , cause(cause)
{
}

QString Exception::causeString() const
{
   if (Cause::NoGit == cause)
      return "NoGit";
   else if (Cause::MalformedUrl == cause)
      return "MalformedUrl";
   else if (Cause::NoCredenialsFile == cause)
      return "NoCredenialsFile";
   else if (Cause::MalformedCredentials == cause)
      return "MalformedCredentials";
   else if (Cause::ServerError == cause)
      return "ServerError";
   else if (Cause::MalformedReply == cause)
      return "MalformedReply";
   else
      return "???";
}
