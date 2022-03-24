#include "Encryption.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

static QByteArray key("GitIssueCredentials");
static QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha1;
static int checkLength = QCryptographicHash::hashLength(algorithm);

QString Encryption::encrypt(const QString& text)
{
   const quint8 random = QRandomGenerator::global()->bounded(255);
   const QByteArray randomByte = QByteArray(reinterpret_cast<const char*>(&random), 1);

   QByteArray data = randomByte + text.toUtf8();

   const quint32 length = text.count();
   quint8 lastChar = 0;

   for (quint32 pos = 0; pos < length; pos++)
   {
      const quint8 keyByte = key.at(pos % key.length());

      quint8 current = data.at(pos);
      lastChar = current ^ keyByte ^ lastChar;
      data[pos] = lastChar;
   }

   data = data.toBase64();
   return QString::fromUtf8(data);
}

QString Encryption::decrypt(QString text)
{
   QByteArray data = text.toUtf8();
   data = QByteArray::fromBase64(data);

   const quint32 length = data.count();
   quint8 lastChar = 0;

   for (quint32 pos = 0; pos < length; pos++)
   {
      const quint8 keyByte = key.at(pos % key.length());

      quint8 current = data.at(pos);
      data[pos] = current ^ lastChar ^ keyByte;
      lastChar = current;
   }

   data = data.mid(1); // discard random byte

   return QString::fromUtf8(data);
}
