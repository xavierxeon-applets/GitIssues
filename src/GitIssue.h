#ifndef GitIssueH
#define GitIssueH

#include <QList>

struct Issue
{
   QString title;
   QString body;
   int number;

   bool operator<(const Issue& other) const;
   using List = QList<Issue>;
};

#endif // GitIssueH
