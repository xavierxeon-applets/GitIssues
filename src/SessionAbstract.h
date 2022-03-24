#ifndef SessionAbstractH
#define SessionAbstractH

#include <QObject>

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QStringList>

#include "Credentials.h"
#include "Exception.h"
#include "GitIssue.h"

namespace Session
{
   class Abstract : public QObject
   {
      Q_OBJECT
   public:
      static Abstract* create();
      void printOpenIssues();
      virtual void createNewIssue(const QString& title) = 0;

   protected:
      Abstract(const QString& gitUrl);

   protected:
      virtual Issue::List openIssues() = 0;
      QJsonArray getEndpoint(const QUrl& endPointUrl);
      QJsonObject postEndpoint(const QUrl& endPointUrl, const QByteArray& payload = QByteArray());
      virtual void authorize(QNetworkRequest& request) const = 0;

   protected:
      const QString gitUrl;
      QString baseUrl;
      QString owner;
      QString repoName;
      Credentials credentials;

   private:
      void parseGitUrl();
      //void readGitCredentials();

   private:
      QNetworkAccessManager* client;
   };
} // namespace Session

#endif // SessionAbstractH
