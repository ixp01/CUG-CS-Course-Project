#include "treenode.h"

TreeNode::TreeNode(QObject *parent)
    : QAbstractItemModel(parent)
{}

QVariant TreeNode::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex TreeNode::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex TreeNode::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int TreeNode::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int TreeNode::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant TreeNode::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
