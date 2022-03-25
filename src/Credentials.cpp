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
   QProcess process;
   process.start("git", {"credential", "fill"});
   process.waitForStarted();

   const QString protocoll = QString("protocol=%1\n").arg(gitUrl.scheme());
   process.write(protocoll.toUtf8());

   const QString host = QString("host=%1\n").arg(gitUrl.host());
   process.write(host.toUtf8());

   const QString path = QString("path=%1\n").arg(gitUrl.path().mid(1));
   process.write(path.toUtf8());

   if (!gitUrl.userName().isEmpty())
   {
      const QString repoUser = QString("username=%1\n").arg(gitUrl.userName());
      process.write(repoUser.toUtf8());
   }

   process.write("\n");
   process.waitForFinished();

   const QString reply = QString::fromUtf8(process.readAllStandardOutput());
   for (const QString& line : reply.split("\n", Qt::SkipEmptyParts))
   {
      int index = line.indexOf("=");
      if (index < 0)
         continue;
      const QString key = line.mid(0, index);
      const QString value = line.mid(index + 1);
      if ("username" == key)
         userName = value;
      if ("password" == key)
         token = value;
   }
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
