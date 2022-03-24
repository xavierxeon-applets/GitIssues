#include "Credentials.h"

#include <iostream>

#include <QProcess>

#include "Encryption.h"
#include "Exception.h"
#include "Settings.h"

Credentials::Credentials()
   : useGitCredentials(false)
{
   //git config --global --get credential.helper
   QProcess process;
   process.start("git", {"config", "--global", "--get", "credential.helper"});
   process.waitForFinished();

   const QString helper = process.readAllStandardOutput();
   if (!helper.isEmpty())
      useGitCredentials = true;
}

void Credentials::read(const QUrl& gitUrl)
{
   if (useGitCredentials)
      readGit(gitUrl);
   else
      readFile(gitUrl);
}

void Credentials::readGit(const QUrl& gitUrl)
{
}

void Credentials::readFile(const QUrl& gitUrl)
{
   const QString host = gitUrl.host();
   QJsonObject object = Settings::object(host);

   if (!object.contains("UserName"))
   {
      std::cout << "provide USER NAME for " << host.toStdString() << ": ";
      std::string inputUserName;
      std::cin >> inputUserName;

      object["UserName"] = QString::fromStdString(inputUserName);
   }

   if (!object.contains("Token"))
   {
      std::cout << "provide TOKEN for " << host.toStdString() << ": ";
      std::string inputToken;
      std::cin >> inputToken;

      QString newToken = QString::fromStdString(inputToken);
      QString encryptedToken = Encryption::encrypt(newToken);

      //object["Token"] = encryptedToken;
      object["Token"] = newToken;
   }

   userName = object["UserName"].toString();
   const QString tokenData = object["Token"].toString();
   //token = Encryption::decrypt(tokenData);
   token = tokenData;

   Settings::write(host, object);
}
