#include "Credentials.h"

#include <iostream>

#include <QFile>
#include <QStandardPaths>

#include "Encryption.h"
#include "Exception.h"
#include "Settings.h"

Credentials::Credentials()
   : source(Source::OwnFile)
   , credentialsFileName()
{
   const QString sourceName = Settings::text("Source");

   if (sourceName.isEmpty())
   {
      source = determineSource();
      saveSource();
   }
   else if ("Keychain" == sourceName)
      source = Source::Keychain;
   else if ("GitCredentialsFile" == sourceName)
      source = Source::GitCredentialsFile;
}

void Credentials::read(const QString& baseUrl)
{
   if (Source::Keychain == source)
      readKeychain(baseUrl);
   else if (Source::GitCredentialsFile == source)
      readGitFile(baseUrl);
   else
      readOwnFile(baseUrl);
}

void Credentials::readKeychain(const QString& baseUrl)
{
}

void Credentials::readGitFile(const QString& baseUrl)
{
   QFile file(credentialsFileName);
   if (!file.open(QIODevice::ReadOnly))
      throw Exception(Exception::Cause::NoCredentialsFile);

   while (!file.atEnd())
   {
      QString line = QString::fromUtf8(file.readLine()).simplified();
      if (!line.endsWith("@" + baseUrl))
         continue;

      line.replace("https://", "");
      line.replace("@" + baseUrl, "");

      QStringList components = line.split(":", Qt::SkipEmptyParts);
      if (components.count() < 2)
         throw Exception(Exception::Cause::MalformedCredentials);

      userName = components.at(0);
      token = components.at(1);
   }

   file.close();
}

void Credentials::readOwnFile(const QString& baseUrl)
{
   QJsonObject object = Settings::object(baseUrl);
   if (!object.contains("UserName"))
   {
      std::cout << "provide USER NAME for " << baseUrl.toStdString() << ": ";
      std::string inputUserName;
      std::cin >> inputUserName;

      object["UserName"] = QString::fromStdString(inputUserName);
   }

   if (!object.contains("Token"))
   {
      std::cout << "provide TOKEN for " << baseUrl.toStdString() << ": ";
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

   Settings::write(baseUrl, object);
}

Credentials::Source Credentials::determineSource()
{
   // test keychain

   // test git credentials
   const QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

   credentialsFileName = homePath + "/.git-credentials-wsl";
   if (QFile::exists(credentialsFileName))
      return Source::GitCredentialsFile;

   credentialsFileName = homePath + "/.git-credentials";
   QFile file(credentialsFileName);
   if (file.open(QIODevice::ReadOnly))
      return Source::GitCredentialsFile;

   // use own solution
   return Source::OwnFile;
}

void Credentials::saveSource()
{
   if (Source::Keychain == source)
      Settings::write("Source", "Keychain");
   if (Source::GitCredentialsFile == source)
      Settings::write("Source", "GitCredentialsFile");
   else
      Settings::write("Source", "OwnFile");
}
