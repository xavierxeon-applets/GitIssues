#include "SessionGitLab.h"

#include <QJsonObject>
#include <QUrlQuery>

Session::GitLab::GitLab(const QString& gitUrl)
   : Abstract(gitUrl)
   , projectId()
{
   QUrl endPointUrl("https://" + baseUrl + "/api/v4/projects");

   for (const QJsonValue& value : getEndpoint(endPointUrl))
   {
      const QJsonObject projectObject = value.toObject();
      const QString projectUrl = projectObject["http_url_to_repo"].toString();
      if (projectUrl != gitUrl)
         continue;

      const int iValue = projectObject["id"].toInt();
      projectId = QString::number(iValue);
      break;
   }
}

Issue::List Session::GitLab::openIssues()
{
   Issue::List issueList;

   QUrlQuery query;
   query.addQueryItem("state", "opened");

   QUrl endPointUrl("https://" + baseUrl + "/api/v4/projects/" + projectId + "/issues");
   endPointUrl.setQuery(query);

   for (const QJsonValue& value : getEndpoint(endPointUrl))
   {
      const QJsonObject issueObject = value.toObject();

      const QString title = issueObject["title"].toString();
      const QString body = issueObject["body"].toString();
      const int number = issueObject["iid"].toInt();

      Issue issue = {title, body, number};
      issueList.append(issue);
   }

   std::sort(issueList.begin(), issueList.end());
   return issueList;
}

void Session::GitLab::createNewIssue(const QString& title)
{
   QUrlQuery query;
   query.addQueryItem("title", title);

   QUrl endPointUrl("https://" + baseUrl + "/api/v4/projects/" + projectId + "/issues");
   endPointUrl.setQuery(query);

   const QJsonObject reply = postEndpoint(endPointUrl);
}

void Session::GitLab::authorize(QNetworkRequest& request) const
{
   request.setRawHeader(QByteArrayLiteral("PRIVATE-TOKEN"), credentials.token.toUtf8());
}
