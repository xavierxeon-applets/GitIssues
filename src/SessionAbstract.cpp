#include "SessionAbstract.h"

#include <QAuthenticator>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QProcess>

#include "SessionGitHub.h"
#include "SessionGitLab.h"

// exception

Session::Exception::Exception(const Cause& cause)
   : QException()
   , cause(cause)
{
}

// Issue

bool Session::Issue::operator<(const Issue& other) const
{
   return (number < other.number);
}

// session

Session::Abstract* Session::Abstract::createSession()
{
   // get remote url
   QProcess processUrl;
   processUrl.start("git", {"config", "--get", "remote.origin.url"});
   if (!processUrl.waitForStarted())
   {
      //qWarning() << process.readAllStandardError().simplified();
      throw Exception(Exception::Cause::NoGit);
   }
   processUrl.waitForFinished();
   const QUrl gitUrl(processUrl.readAllStandardOutput().simplified());

   // no url means no issue server
   if (gitUrl.isEmpty())
      throw Exception(Exception::Cause::NoRemote);

   // test if credential helper available
   QProcess processCredentialHelper;
   processCredentialHelper.start("git", {"config", "--global", "--get", "credential.helper"});
   processCredentialHelper.waitForFinished();

   const QString helper = processCredentialHelper.readAllStandardOutput();
   if (helper.isEmpty())
      throw Exception(Exception::Cause::NoCredentialHelper);

   if (gitUrl.host().contains("github.com"))
      return new GitHub(gitUrl);
   else
      return new GitLab(gitUrl);
}

Session::Abstract::Abstract(const QUrl& gitUrl)
   : QObject(nullptr)
   , gitUrl(gitUrl)
   , host()
   , owner()
   , repoName()
   , userName()
   , token()
   , client(nullptr)
{
   client = new QNetworkAccessManager(this);

   host = gitUrl.host();

   const QStringList components = gitUrl.path().split("/");
   owner = components.at(1);
   repoName = components.mid(2).join("/");
   repoName.replace(".git", "");

   const QString protocoll = QString("protocol=%1\n").arg(gitUrl.scheme());
   const QString host = QString("host=%1\n").arg(gitUrl.host());
   const QString path = QString("path=%1\n").arg(gitUrl.path().mid(1));
   const QString repoUser = QString("username=%1\n").arg(gitUrl.userName());

   QProcess processGetCredentials;
   processGetCredentials.start("git", {"credential", "fill"});
   processGetCredentials.waitForStarted();

   processGetCredentials.write(protocoll.toUtf8());
   processGetCredentials.write(host.toUtf8());
   processGetCredentials.write(path.toUtf8());
   if (!gitUrl.userName().isEmpty())
      processGetCredentials.write(repoUser.toUtf8());

   processGetCredentials.write("\n");
   processGetCredentials.waitForFinished();

   const QString reply = QString::fromUtf8(processGetCredentials.readAllStandardOutput());
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

   // qDebug() << userName << token;
}

QJsonArray Session::Abstract::getEndpoint(const QUrl& endPointUrl)
{
   QNetworkRequest request(endPointUrl);
   authorize(request);

   QNetworkReply* reply = client->get(request);

   while (!reply->isFinished())
      QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

   const QByteArray data = reply->readAll();
   const QNetworkReply::NetworkError error = reply->error();

   reply->deleteLater();
   if (QNetworkReply::NoError != error)
   {
      qWarning() << error << data;
      throw Exception(Exception::Cause::ServerError);
   }

   QJsonParseError parserError;
   QJsonDocument doc = QJsonDocument::fromJson(data, &parserError);
   if (QJsonParseError::NoError != parserError.error)
      throw Exception(Exception::Cause::MalformedReply);

   const QJsonArray array = doc.array();
   return array;
}

QJsonObject Session::Abstract::postEndpoint(const QUrl& endPointUrl, const QByteArray& payload)
{
   QNetworkRequest request(endPointUrl);
   authorize(request);

   QNetworkReply* reply = client->post(request, payload);

   while (!reply->isFinished())
      QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

   const QByteArray data = reply->readAll();
   const QNetworkReply::NetworkError error = reply->error();

   reply->deleteLater();
   if (QNetworkReply::NoError != error)
   {
      qWarning() << error << data;
      throw Exception(Exception::Cause::ServerError);
   }

   QJsonParseError parserError;
   QJsonDocument doc = QJsonDocument::fromJson(data, &parserError);
   if (QJsonParseError::NoError != parserError.error)
      throw Exception(Exception::Cause::MalformedReply);

   const QJsonObject object = doc.object();
   return object;
}
