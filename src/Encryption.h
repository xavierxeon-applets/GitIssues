#ifndef EncryptionH
#define EncryptionH

#include <QString>

// uses a simple XOR cipher + some additional obfuscations
// see https://en.wikipedia.org/wiki/XOR_cipher
// THIS IS NOT A SAFE ENCRYPTION!

struct Encryption
{
   static QString encrypt(const QString& text);
   static QString decrypt(QString text);
};

#endif // EncryptionH
