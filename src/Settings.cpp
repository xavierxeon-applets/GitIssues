#include "Settings.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStandardPaths>

static const QString settingsFileName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.gitissues.json";

struct Data
{
   Data();
   ~Data();

   QJsonObject settingsObject;
};

Data::Data()
{
   QFile file(settingsFileName);
   if (!file.open(QIODevice::ReadOnly))
      return;

   const QByteArray content = file.readAll();
   file.close();

   QJsonParseError parserError;
   QJsonDocument doc = QJsonDocument::fromJson(content, &parserError);

   settingsObject = doc.object();
}

Data::~Data()
{
   QFile file(settingsFileName);
   if (!file.open(QIODevice::WriteOnly))
      return;

   QJsonDocument doc(settingsObject);
   const QByteArray content = doc.toJson();

   file.write(content);
   file.close();
}

// settings

QString Settings::text(const QString& key)
{
   Data data;
   if (data.settingsObject.contains(key))
      return data.settingsObject[key].toString();
   else
      return QString();
}

QJsonObject Settings::object(const QString& key)
{
   Data data;
   if (data.settingsObject.contains(key))
      return data.settingsObject[key].toObject();
   else
      return QJsonObject();
}

void Settings::write(const QString& key, const QString& text)
{
   Data data;
   data.settingsObject[key] = text;
}

void Settings::write(const QString& key, const QJsonObject& object)
{
   Data data;
   data.settingsObject[key] = object;
}
