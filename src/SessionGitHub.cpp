#include "SessionGitHub.h"

#include <QJsonDocument>
#include <QJsonObject>

Session::GitHub::GitHub(const QUrl& gitUrl)
   : Abstract(gitUrl)
{
}

Issue::List Session::GitHub::openIssues()
{
   Issue::List issueList;

   QUrl endPointUrl("https://api.github.com/repos/" + owner + "/" + repoName + "/issues");

   for (const QJsonValue& value : getEndpoint(endPointUrl))
   {
      const QJsonObject issueObject = value.toObject();

      const QString title = issueObject["title"].toString();
      const QString body = issueObject["body"].toString();
      const int number = issueObject["number"].toInt();

      Issue issue = {title, body, number};
      issueList.append(issue);
   }

   std::sort(issueList.begin(), issueList.end());
   return issueList;
}

void Session::GitHub::createNewIssue(const QString& title)
{
   QJsonObject payloadObject;
   payloadObject["title"] = title;
   QJsonDocument doc(payloadObject);

   QUrl endPointUrl("https://api.github.com/repos/" + owner + "/" + repoName + "/issues");

   const QByteArray payload = doc.toJson(QJsonDocument::Compact);
   postEndpoint(endPointUrl, payload);
}

void Session::GitHub::authorize(QNetworkRequest& request) const
{
   const QByteArray content = QByteArray("Basic ") + (credentials.userName + ":" + credentials.token).toUtf8().toBase64();
   request.setRawHeader(QByteArrayLiteral("Authorization"), content);
}
