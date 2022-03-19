#include "wjtreeview.h"

using namespace std;

WJTREEVIEW::WJTREEVIEW()
{

}

void WJTREEVIEW::setModel(const std::shared_ptr<Wt::WAbstractItemModel>& waiModel)
{
    if (!columnWidthChanged_.isConnected())
        columnWidthChanged_.connect(this, &WAbstractItemView::updateColumnWidth);

    bool isReset = model_.get();

    /* disconnect slots from previous model */
    for (unsigned i = 0; i < modelConnections_.size(); ++i)
        modelConnections_[i].disconnect();
    modelConnections_.clear();

    model_ = model;
    headerModel_.reset(new HeaderProxyModel(model_));

    auto oldSelectionModel = std::move(selectionModel_);
    selectionModel_.reset(new WItemSelectionModel(model));
    selectionModel_->setSelectionBehavior(oldSelectionModel->selectionBehavior());

    delayedClearAndSelectIndex_ = WModelIndex();

    editedItems_.clear();

    if (!isReset)
        initDragDrop();

    configureModelDragDrop();

    setRootIndex(WModelIndex());

    setHeaderHeight(headerLineHeight_);
}
