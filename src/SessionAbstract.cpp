#include "SessionAbstract.h"

#include <QAuthenticator>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QProcess>

#include "SessionGitHub.h"
#include "SessionGitLab.h"

// issue

bool Issue::operator<(const Issue& other) const
{
   return (number < other.number);
}

// session abract

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

void Session::Abstract::printOpenIssues()
{
   const Issue::List openIssueList = openIssues();
   if (openIssueList.isEmpty())
   {
      qInfo() << "no open issues";
   }
   else
   {
      qInfo() << "open issues:";
      for (const Issue& issue : openIssueList)
         qInfo() << issue.number << ": " << qPrintable(issue.title);
   }
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
   host = gitUrl.host();

   const QStringList components = gitUrl.path().split("/");
   owner = components.at(1);
   repoName = components.mid(2).join("/");
   repoName.replace(".git", "");

   credentials.read(gitUrl);

   client = new QNetworkAccessManager(this);
}

bool Session::Abstract::testGitRepository()
{
   QProcess process;
   process.start("git", {"config", "--get", "remote.origin.url"});
   if (!process.waitForStarted())
   {
      //qWarning() << process.readAllStandardError().simplified();
      throw Exception(Exception::Cause::NoGit);
   }
   process.waitForFinished();
}

bool Session::Abstract::testCredentialHelper()
{
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
