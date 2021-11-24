#ifndef SessionGitLabH
#define SessionGitLabH

#include "SessionAbstract.h"

namespace Session
{
   class GitLab : public Abstract
   {
      Q_OBJECT
   public:
      GitLab(const QString& gitUrl);

   private:
      Issue::List openIssues() override;
      void createNewIssue(const QString& title) override;
      void authorize(QNetworkRequest& request) const override;

   private:
      QString projectId;
   };
} // namespace Session

#endif // SessionGitLabH
