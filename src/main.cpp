#include "SessionAbstract.h"

#include <QCoreApplication>

// main function

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   const QStringList argList = app.arguments();

   try
   {
      Session::Abstract* session = Session::Abstract::createSession();
      if (1 == argList.count())
      {
         session->printOpenIssues();
      }
      else
      {
         const QString title = argList.mid(1).join(" ");
         session->createNewIssue(title);
      }
   }
   catch (const Exception& e)
   {
      qCritical() << e.causeString();
   }

   return 0;
}
