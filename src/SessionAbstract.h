#ifndef SessionAbstractH
#define SessionAbstractH

#include <QException>
#include <QObject>

#include <QNetworkAccessManager>

namespace Session
{
   class Exception : public QException
   {
   public:
      enum class Cause
      {
         NoGit,
         NoRemote,
         NoCredentialHelper,
         ServerError,
         MalformedUrl,
         MalformedReply,
         ProjectNotFound
      };

   public:
      Exception(const Cause& cause);

   public:
      const Cause cause;
   };

   struct Issue
   {
      QString title;
      QString body;
      int number;

      bool operator<(const Issue& other) const;
      using List = QList<Issue>;
   };

   class Abstract : public QObject
   {
      Q_OBJECT
   public:
      static Abstract* createSession();
      virtual void createNewIssue(const QString& title) = 0;
      virtual Issue::List openIssues() = 0;
      virtual QUrl getIssuesPageUrl() const = 0;

   protected:
      Abstract(const QUrl& gitUrl);

   protected:
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
