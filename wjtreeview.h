#pragma once
#include <Wt/WAbstractItemView.h>

class WJTREEVIEW : public Wt::WAbstractItemView
{
public:
	WJTREEVIEW();
	~WJTREEVIEW() = default;

	void setModel(const std::shared_ptr<Wt::WAbstractItemModel>& waiModel) override;
};