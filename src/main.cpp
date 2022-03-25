#include "SessionAbstract.h"

#include <QCommandLineParser>
#include <QDesktopServices>
#include <QGuiApplication>

// main function

int main(int argc, char** argv)
{
   QGuiApplication app(argc, argv);
   QGuiApplication::setApplicationName("GitIssue");
   QGuiApplication::setApplicationVersion("1.0");

   QCommandLineParser parser;
   parser.setApplicationDescription("add and review issues on GitHub and GitLab");
   parser.addPositionalArgument("title", "add an issue with the given title (no quotation marks neccessary).");

   QCommandLineOption browserOption("open", "Open the browser and show the project issue page.");
   parser.addOption(browserOption);

   parser.process(app);

   const QString title = parser.positionalArguments().join(" ");
   const bool openBrowser = parser.isSet(browserOption);

   using Session::Exception;
   using Session::Issue;

   try
   {
      Session::Abstract* session = Session::Abstract::createSession();

      if (openBrowser)
      {
         const QUrl url = session->getIssuesPageUrl();
         QDesktopServices::openUrl(url);
         qInfo() << "open browser";
      }
      else if (title.isEmpty())
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
               qInfo() << "   " << issue.number << ": " << qPrintable(issue.title);
            qInfo() << "use \"GitIssue --open\" to open issues in browser";
         }
      }
      else
      {
         session->createNewIssue(title);
         qInfo() << "created issue";
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
      else if (Exception::Cause::ProjectNotFound == exception.cause)
         qCritical() << "ProjectNotFound";
      else
         qCritical() << "???";
   }

   return 0;
}
