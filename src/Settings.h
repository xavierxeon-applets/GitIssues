#ifndef SettingsH
#define SettingsH

#include <QJsonObject>

class Settings
{
public:
   static QString text(const QString& key);
   static QJsonObject object(const QString& key);
   static void write(const QString& key, const QString& text);
   static void write(const QString& key, const QJsonObject& object);
};

#endif // SettingsH
