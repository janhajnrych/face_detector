#include "../include/image_list_model.h"


ImageListModel::ImageListModel(QObject *parent): QAbstractListModel(parent) {}

ImageListModel::~ImageListModel() {}

void ImageListModel::addImage(QImage image, QString name) {
    auto parent = QModelIndex();
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    Item item;
    item.name = name;
    item.thumbnail = QPixmap::fromImage(image.scaled(100, 100, Qt::KeepAspectRatio));
    items.push_back(item);
    endInsertRows();
}

// void ImageListModel::removeImage(const QString& name) {
//     if (idMap.find(name.toStdString()) == idMap.end())
//         return;
//     auto parent = QModelIndex();
//     auto id = index.row();
//     beginRemoveRows(parent, id, id);
//     items.removeAt(id);
//     idMap.erase(name.toStdString());
//     endRemoveRows();
// }

void ImageListModel::removeImage(const QString name) {
    auto parent = QModelIndex();
    auto it = std::find_if(items.begin(), items.end(), [name](auto item){
        return item.name == name;
    });
    if (it == items.end())
        return;
    auto id = std::distance(items.begin(), it);
    beginRemoveRows(parent, id, id);
    items.removeAt(id);
    endRemoveRows();
}

QString ImageListModel::getName(const QModelIndex& index) const {
    auto id = index.row();
    return items.at(id).name;
}

int ImageListModel::rowCount(const QModelIndex &parent) const{
    if (parent.isValid())
        return 0;
    else
        return items.size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
            case Qt::DecorationRole:
            {
                return items.at(index.row()).thumbnail;
            }
            case Qt::DisplayRole:
            {
                return items.at(index.row()).name;
            }
        }
    }
    return QVariant();
}


