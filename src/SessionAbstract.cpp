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

Session::Abstract::Abstract(const QString& gitUrl)
   : QObject(nullptr)
   , gitUrl(gitUrl)
   , baseUrl()
   , owner()
   , repoName()
   , credentials()
   , client(nullptr)
{
   parseGitUrl();
   credentials.read(baseUrl);

   client = new QNetworkAccessManager(this);
}

Session::Abstract* Session::Abstract::create()
{
   QProcess process;
   process.start("git", {"config", "--get", "remote.origin.url"});
   if (!process.waitForStarted())
   {
      qWarning() << process.readAllStandardError().simplified();
      throw Exception(Exception::Cause::NoGit);
   }

   process.waitForFinished();

   const QString gitUrl = process.readAllStandardOutput().simplified();
   if (gitUrl.contains("github.com"))
      return new GitHub(gitUrl);
   else
      return new GitLab(gitUrl);
}

void Session::Abstract::printOpenIssues()
{
   const Issue::List openIssueList = openIssues();
   if (openIssueList.isEmpty())
   {
      qDebug() << "no open issues";
   }
   else
   {
      qDebug("open issues:");
      for (const Issue& issue : openIssueList)
         qDebug() << issue.number << ": " << qPrintable(issue.title);
   }
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

void Session::Abstract::parseGitUrl()
{
   QString content = gitUrl;
   content.replace("https://", "");
   content.replace(".git", "");

   QStringList components = content.split("/", Qt::SkipEmptyParts);
   if (components.count() < 3)
      throw Exception(Exception::Cause::MalformedUrl);

   baseUrl = components.at(0);
   if (baseUrl.contains("@"))
   {
      int index = baseUrl.indexOf("@") + 1;
      baseUrl = baseUrl.mid(index);
   }
   owner = components.at(1);
   repoName = components.mid(2).join("/");
}
