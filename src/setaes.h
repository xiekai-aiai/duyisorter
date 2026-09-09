#ifndef SETAES_H
#define SETAES_H

#include <string.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

using namespace std;


class SETAES
{
public:
    SETAES();
    int Base64Encode(const char *encoded, int encodedLength, char *decoded);
    int Base64Decode(const char *encoded, int encodedLength, char *decoded);
    string EncryptAES(const string& strKey, const string& strData);
    string DecryptAES(const string& strKey, const string& strDecryBase64Data);
    int  numberDecrypt(int cipherText, int key);

};


#endif // SETAES_H
