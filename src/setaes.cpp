#include "setaes.h"

const int BUFFER_SIZE = 1024;

SETAES::SETAES()
{

}

// base64编码
int SETAES::Base64Encode(const char *encoded, int encodedLength, char *decoded)
{
    return EVP_EncodeBlock((unsigned char*)decoded, (const unsigned char*)encoded, encodedLength);
}

// base解码
int SETAES::Base64Decode(const char *encoded, int encodedLength, char *decoded)
{
    return EVP_DecodeBlock((unsigned char*)decoded, (const unsigned char*)encoded, encodedLength);
}

// AES算法加密
string SETAES::EncryptAES(const string& strKey, const string& strData)
{
    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char*)strKey.c_str(), strKey.length() * 8, &aes_key) < 0)
    {
        assert(false);
        return "";
    }

    string strEncryptedData;
    string strDataBak = strData;
    unsigned int data_length = strDataBak.length();
    int padding = 0;
    if (strDataBak.length() % AES_BLOCK_SIZE > 0)
    {
        padding = AES_BLOCK_SIZE - strDataBak.length() % AES_BLOCK_SIZE;
    }
    data_length += padding;
    while (padding > 0)
    {
        strDataBak += '\0';
        padding--;
    }
    for (unsigned int i = 0; i < data_length / AES_BLOCK_SIZE; i++)
    {
        string strBlock = strDataBak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);
        AES_encrypt((const unsigned char*)strBlock.c_str(), out, &aes_key);
        strEncryptedData += string((const char*)out, AES_BLOCK_SIZE);
    }

    char buffer[BUFFER_SIZE];
    Base64Encode(strEncryptedData.c_str(), strEncryptedData.size(),buffer);
    return string(buffer,strlen(buffer));
}

// AES算法解密
string SETAES::DecryptAES(const string& strKey, const string& strDecryBase64Data)
{
    char buffer[BUFFER_SIZE];
    Base64Decode(strDecryBase64Data.c_str(), strDecryBase64Data.size(), buffer);
    string strEncryData(buffer, strlen(buffer));
    AES_KEY aes_key;
    if (AES_set_decrypt_key((const unsigned char*)strKey.c_str(), strKey.length() * 8, &aes_key) < 0)
    {
        assert(false);
        return "";
    }
    std::string strRet;
    for (unsigned int i = 0; i < strEncryData.length() / AES_BLOCK_SIZE; i++)
    {
        std::string strBlock = strEncryData.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);
        AES_decrypt((const unsigned char*)strBlock.c_str(), out, &aes_key);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    string::size_type pos = strRet.find_last_not_of('\0');
    if (pos != string::npos)
    {
        strRet = strRet.substr(0, pos + 1);
    }
    return strRet;
}

// 数字解密
int SETAES::numberDecrypt(int cipherText, int key)
{
    return cipherText ^ key;
}
