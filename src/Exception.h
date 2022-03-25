#ifndef ExceptionH
#define ExceptionH

#include <QException>

class Exception : public QException
{
public:
   enum class Cause
   {
      NoGit,
      NoRemote,
      NoCredentialHelper,
      ServerError,
      MalformedUrl,
      MalformedReply
   };

public:
   Exception(const Cause& cause);

public:
   QString causeString() const;

private:
   const Cause cause;
};

#endif // ExceptionH
