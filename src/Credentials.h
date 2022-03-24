#ifndef CredentialsH
#define CredentialsH

#include <QString>

class Credentials
{
public:
   Credentials();

public:
   void read(const QString& baseUrl);

public:
   QString userName;
   QString token;

private:
   enum class Source
   {
      GitCredentials,
      IssueSettings
   };

private:
   void readGit(const QString& baseUrl);
   void readFile(const QString& baseUrl);
   Source determineSource();
   void saveSource();

private:
   Source source;
};

#endif // CredentialsH
