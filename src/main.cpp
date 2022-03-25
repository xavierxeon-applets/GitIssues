#include "SessionAbstract.h"

#include <QCoreApplication>

// main function

int main(int argc, char** argv)
{
   QCoreApplication app(argc, argv);
   const QStringList argList = app.arguments();

   using Session::Exception;
   using Session::Issue;

   try
   {
      Session::Abstract* session = Session::Abstract::createSession();
      if (1 == argList.count())
      {
         const Issue::List openIssueList = session->openIssues();
         if (openIssueList.isEmpty())
         {
            qInfo() << "no open issues";
         }
         else
         {
            qInfo() << "open issues:";
            for (const Issue& issue : openIssueList)
               qInfo() << issue.number << ": " << qPrintable(issue.title);
         }
      }
      else
      {
         const QString title = argList.mid(1).join(" ");
         session->createNewIssue(title);
      }
   }
   catch (const Exception& exception)
   {
      if (Exception::Cause::NoGit == exception.cause)
         qCritical() << "NoGit";
      else if (Exception::Cause::NoRemote == exception.cause)
         qCritical() << "NoRemote";
      else if (Exception::Cause::NoCredentialHelper == exception.cause)
         qCritical() << "NoCredentialHelper";
      else if (Exception::Cause::ServerError == exception.cause)
         qCritical() << "ServerError";
      else if (Exception::Cause::MalformedUrl == exception.cause)
         qCritical() << "MalformedUrl";
      else if (Exception::Cause::MalformedReply == exception.cause)
         qCritical() << "MalformedReply";
      else
         qCritical() << "???";
   }

   return 0;
}
