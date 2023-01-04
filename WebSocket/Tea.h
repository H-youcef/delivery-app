#ifndef TEA_H
#define TEA_H

#include <cinttypes>
#include <array>
#include <QByteArray>
#include <exception>
#include <vector>
#include <QtGlobal>
#include <QString>
#include <algorithm>
#include <QDebug>

class Tea {

public:
    using byte = uint8_t;
    using ByteArray = std::vector<byte>;
    using Uint64Array = std::vector<uint64_t>;
    using Uint32Array = std::vector<uint32_t>;

    Tea() = default;
    /**
     * Initialize the cipher for encryption or decryption.
     * @param key a 16 byte (128-bit) key
     */
    Tea(const QString &key){
        setKey(key);
    }

    Tea(const ByteArray &key) {
        if (key.empty())
            throw std::invalid_argument("Invalid key: Key was null");

        if (key.size() < 16)
            throw std::invalid_argument("Invalid key: Length was less than 16 bytes");

        init(key);
    }
    bool hasKey()const{return m_hasKey;}

    void setKey(const QString &key){
        if (key.isEmpty()){
            m_hasKey = false;
            return;
        }

        if (key.length() < 32)
            throw std::invalid_argument("Invalid key string: Length was less than 32 symboles");

        ByteArray keyBytes;
        QString tmp(2, QChar{});
        for(int i = 0; i < 32; i += 2){
            tmp[0] = key[i];
            tmp[1] = key[i + 1];
            bool ok = false;
            byte b = static_cast<byte>(tmp.toUInt(&ok, 16) & 0xff);
            if(!ok){
                std::invalid_argument("Invalid key string: it contains non hex symbols");
            }
            keyBytes.push_back(b);
        }
        init(keyBytes);
    }
    /**
     * Encrypt an array of bytes.
     * @param clear the cleartext to encrypt
     * @return the encrypted text
     */
    QByteArray encrypt(const QString &clear) {
        auto qbytes = clear.toUtf8();
        ByteArray bytes(size_t(qbytes.size()));
        std::copy(qbytes.begin(), qbytes.end(), bytes.begin());
        bytes = encrypt(bytes);
        qbytes.resize(int(bytes.size()));
        std::copy(bytes.begin(), bytes.end(), qbytes.begin());
        return qbytes;
    }
    /**
     * Encrypt an array of bytes.
     * @param clear the cleartext to encrypt
     * @return the encrypted text
     */
    ByteArray encrypt(const ByteArray &clear) {
        if(!m_hasKey){
            qDebug() << "encrypt: No key is supplied returning data as is";
            return clear;
        }
        size_t paddedSize = ((clear.size() / 8) + (((clear.size() % 8) == 0) ? 0 : 1)) * 2;
        Uint32Array buffer;
        buffer.resize(paddedSize + 1);
        initArrToZeros(buffer);
        buffer[0] = static_cast<uint32_t>(clear.size());
        pack(clear, buffer, 1);
        brew(buffer);
        return unpack(buffer, 0, buffer.size() * 4);
    }

    /**
     * Decrypt an array of bytes.
     * @param ciper the cipher text to decrypt
     * @return the decrypted text
     */
    QString decrypt(const QByteArray &crypt) {
        ByteArray bytes(size_t(crypt.length()));
        std::copy(crypt.begin(), crypt.end(), bytes.begin());
        bytes = decrypt(bytes);
        QByteArray qbytes;
        qbytes.resize(int(bytes.size()));
        std::copy(bytes.begin(), bytes.end(), qbytes.begin());
        return QString(qbytes);
    }


    /**
     * Decrypt an array of bytes.
     * @param ciper the cipher text to decrypt
     * @return the decrypted text
     */
    ByteArray decrypt(const ByteArray &crypt) {
        if(!m_hasKey){
            qDebug() << "decrypt: No key is supplied returning data as is";
            return crypt;
        }
        if(crypt.size() % 4 != 0){
            throw std::invalid_argument("decrypt: bad args (crypt.size() % 4 != 0)");
        }
        if((crypt.size() / 4) % 2 != 1){
            throw std::invalid_argument("decrypt: bad args ((crypt.size() / 4) % 2 != 1)");
        }
        Uint32Array buffer;
        buffer.resize(crypt.size() / 4);
        initArrToZeros(buffer);
        pack(crypt, buffer, 0);
        unbrew(buffer);
        return unpack(buffer, 1, buffer[0]);
    }

private: //private methods
    void init(const ByteArray &key) {
        size_t off=0;
        for (size_t i=0; i < 4; i++) {
            uint32_t b1 = uint32_t( key.at(off++) ) & 0xff;
            uint32_t b2 = uint32_t( key.at(off++)   & 0xff ) <<  8;
            uint32_t b3 = uint32_t( key.at(off++)   & 0xff ) <<  16;
            uint32_t b4 = uint32_t( key.at(off++)   & 0xff ) <<  24;

            S[i] = b1 | b2 | b3 | b4;
        }
        m_hasKey = true;
    }

    void brew(Uint32Array &buf) {
        Q_ASSERT(buf.size() % 2 == 1);
        uint32_t v0, v1, sum, n;
        size_t i = 1;

        while (i < buf.size()) {
            n = CUPS;
            v0 = buf[i];
            v1 = buf[i + 1];
            sum = 0;
            while (n-- > 0) {
                sum += SUGAR;
                v0  += ( ( ( v1 << 4 ) + S[0] ) ^ v1 ) + ( sum ^ ( v1 >> 5 ) ) + S[1];
                v1  += ( ( ( v0 << 4 ) + S[2] ) ^ v0 ) + ( sum ^ ( v0 >> 5 ) ) + S[3];
            }
            buf[i]     = v0;
            buf[i + 1] = v1;
            i += 2;
        }
    }

    void unbrew(Uint32Array &buf) {
        Q_ASSERT(buf.size() % 2 == 1);
        size_t i;
        uint32_t v0, v1, sum, n;
        i = 1;
        while (i < buf.size()) {
            n = CUPS;
            v0 = buf[i];
            v1 = buf[i+1];
            sum = UNSUGAR;
            while (n--> 0) {
                v1  -= (((v0 << 4 ) + S[2]) ^ v0) + ( sum ^ ( v0 >> 5 ) ) + S[3];
                v0  -= (((v1 << 4 ) + S[0]) ^ v1) + ( sum ^ ( v1 >> 5 ) ) + S[1];
                sum -= SUGAR;
            }
            buf[i] = v0;
            buf[i + 1] = v1;
            i+=2;
        }
    }

    void pack(const ByteArray &src, Uint32Array &dest, size_t destOffset) {
        Q_ASSERT(destOffset + (src.size() / 4) <= dest.size());
        size_t i = 0, shift = 24;
        size_t j = destOffset;
        dest[j] = 0;
        while (i < src.size()) {
            dest[j] |= static_cast<uint64_t>(src[i] & 0xff) << shift;
            if (shift == 0) {
                shift = 24;
                j++;
                if ( j < dest.size()) dest[j] = 0;
            }
            else {
                shift -= 8;
            }
            i++;
        }
    }

    ByteArray unpack(const Uint32Array &src, size_t srcOffset, size_t destLength) {
        Q_ASSERT(destLength <= (src.size() - srcOffset) * 4);
        std::vector<byte> dest;
        dest.resize(destLength);

        size_t i = srcOffset;
        size_t count = 0;

        for (size_t j = 0; j < destLength; j++) {
            dest[j] = static_cast<byte>( ( src[i] >> (24 - ( 8 * count)) ) & 0xff);
            count++;
            if (count == 4) {
                count = 0;
                i++;
            }
        }
        return dest;
    }

    template<class ARRAY_T>
    void initArrToZeros(ARRAY_T &arr){
        for(size_t i = 0; i < arr.size(); ++i) arr[i] = 0;
    }
private:
    const uint32_t SUGAR = 0x9E3779B9;
    const uint32_t CUPS  = 32;
    const uint32_t UNSUGAR = 0xC6EF3720;
    std::array<uint32_t, 4> S;
    bool m_hasKey = false;
};
#endif // TEA_H
