#ifndef SessionAbstractH
#define SessionAbstractH

#include <QObject>

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QStringList>

#include "Exception.h"

struct Issue
{
   QString title;
   QString body;
   int number;

   bool operator<(const Issue& other) const;
   using List = QList<Issue>;
};

namespace Session
{
   class Abstract : public QObject
   {
      Q_OBJECT
   public:
      static Abstract* createSession();
      void printOpenIssues();
      virtual void createNewIssue(const QString& title) = 0;

   protected:
      Abstract(const QUrl& gitUrl);

   protected:
      virtual Issue::List openIssues() = 0;
      QJsonArray getEndpoint(const QUrl& endPointUrl);
      QJsonObject postEndpoint(const QUrl& endPointUrl, const QByteArray& payload = QByteArray());
      virtual void authorize(QNetworkRequest& request) const = 0;

   protected:
      const QUrl gitUrl;
      QString host;
      QString owner;
      QString repoName;
      QString userName;
      QString token;

   private:
      QNetworkAccessManager* client;
   };
} // namespace Session

#endif // SessionAbstractH
