#ifndef CredentialsH
#define CredentialsH

#include <QString>
#include <QUrl>

class Credentials
{
public:
   Credentials();

public:
   void read(const QUrl& gitUrl);

public:
   QString userName;
   QString token;

private:
   void readGit(const QUrl& gitUrl);
   void readFile(const QUrl& gitUrl);

private:
   bool useGitCredentials;
};

#endif // CredentialsH
