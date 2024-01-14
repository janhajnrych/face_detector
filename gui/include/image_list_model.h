#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H
#include <QAbstractListModel>
#include <QPixmap>
#include <unordered_set>
#include <string>
#include <filesystem>


class ImageListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    struct Item {
        QString name;
        QPixmap thumbnail;
    };
    explicit ImageListModel(QObject *parent = 0);
    virtual ~ImageListModel();
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QString getName(const QModelIndex& parent) const;
public slots:
    void addImage(QImage image, QString name);
    void removeImage(const QString name);
private:
    QList<Item> items;
};

#endif

