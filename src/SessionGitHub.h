#ifndef SessionGitHubH
#define SessionGitHubH

#include "SessionAbstract.h"

namespace Session
{
   class GitHub : public Abstract
   {
      Q_OBJECT
   public:
      GitHub(const QUrl& gitUrl);

   private:
      Issue::List openIssues() override;
      void createNewIssue(const QString& title) override;
      QUrl getIssuesPageUrl() const override;
      void authorize(QNetworkRequest& request) const override;
   };
} // namespace Session

#endif // SessionGitHubH
