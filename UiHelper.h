#ifndef UIHELPER_H
#define UIHELPER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QHash>
#include <QColor>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <cmath>

class UiHelper: public QObject
{
    Q_OBJECT
public:
    explicit UiHelper(QObject *parent = nullptr):QObject(parent){}
    void loadColorsFile(const QString &jsonFilePath){
        QFile file(jsonFilePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Unable to laod colors file: " << file.fileName()
                     << " : " << file.errorString();
            return;
        }
        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        auto objects = jsonDoc.object();
        if(objects.isEmpty()){
            return;
        }
        for(auto it = objects.begin(); it != objects.end(); ++it){
            m_colors.insert(it.key(), it.value().toString());
        }
    }
    Q_INVOKABLE QColor color(const QString &colorName, qreal a=1.0)const{
        QColor c(m_colors.value(colorName));
        c.setAlphaF(a);
        return c;
    }

    Q_INVOKABLE QColor colorFromHash(const QString &str, qreal a=1.0)const{
        static const std::array colorsNames = {
            "pink-a400", "purple-a700",
            "cyan-a700","yellow-a200",
            "light-green-a700", "amber-a400",
            "orange-a700",
        };

        const size_t idx = (static_cast<size_t>(hash(str)) + 0) % colorsNames.size();
        return color(colorsNames.at(idx), a);
    }


    Q_INVOKABLE double dp(double n){
        const auto *const screen = QGuiApplication::screens().at(0);
        return std::round(n * screen->physicalDotsPerInch() / 160.0);
    }
    Q_INVOKABLE int hash(const QString &text)const{
        QString str = text.right(7);
        int hash = 0;
        if (str.length() == 0) return hash;

        for (int i = 0; i < str.length(); i++) {
            char c = str[i].toLatin1();
            hash = ( ( hash << 5 ) - hash) + c;
            hash = hash & hash; // Convert to 32bit integer
        }
        return std::abs(hash) ;
    }

    Q_INVOKABLE QString toTitleCase(const QString &str) {
        if(str.isEmpty())
            return {};
        return str[0].toUpper() + str.right(str.length() - 1).toLower();
    }
private:
    QHash<QString, QColor> m_colors;
};

#endif // UIHELPER_H
