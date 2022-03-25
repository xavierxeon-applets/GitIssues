#include "SessionGitLab.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QUrlQuery>

Session::GitLab::GitLab(const QUrl& gitUrl)
   : Abstract(gitUrl)
   , projectId()
{
   // list all projects
   QUrl endPointUrl("https://" + host + "/api/v4/projects");

   const int projectsPerPage = 200;
   for (int page = 1; true; page++)
   {
      QUrlQuery query;
      query.addQueryItem("page", QString::number(page));
      query.addQueryItem("per_page", QString::number(projectsPerPage));
      endPointUrl.setQuery(query);

      quint8 projectCounter = 0;
      for (const QJsonValue& value : getEndpoint(endPointUrl))
      {
         projectCounter++;
         const QJsonObject projectObject = value.toObject();
         const QString projectUrl = projectObject["http_url_to_repo"].toString();
         if (projectUrl != gitUrl.toString())
            continue;

         const int iValue = projectObject["id"].toInt();
         projectId = QString::number(iValue);
         projectCounter = 0; // to break outer loop
         break;
      }

      if (projectCounter < projectsPerPage)
         break;

      QThread::sleep(1000); // to avoid REST API spam
   }

   if (projectId.isEmpty())
      throw Exception(Exception::Cause::ProjectNotFound);
}

Session::Issue::List Session::GitLab::openIssues()
{
   Issue::List issueList;

   QUrl endPointUrl("https://" + host + "/api/v4/projects/" + projectId + "/issues");

   QUrlQuery query;
   query.addQueryItem("state", "opened");
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

   QUrl endPointUrl("https://" + host + "/api/v4/projects/" + projectId + "/issues");
   endPointUrl.setQuery(query);

   const QJsonObject reply = postEndpoint(endPointUrl);
}

QUrl Session::GitLab::getIssuesPageUrl() const
{
   return QUrl(gitUrl.toString().replace(".git", "/-/issues"));
}

void Session::GitLab::authorize(QNetworkRequest& request) const
{
   request.setRawHeader(QByteArrayLiteral("PRIVATE-TOKEN"), token.toUtf8());
}
