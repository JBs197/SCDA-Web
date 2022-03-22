#pragma once
#include <Wt/WItemDelegate.h>
#include <Wt/WModelIndex.h>

class WJCELL : public Wt::WItemDelegate
{
	std::string cssCell, cssRowHeader;
	const double height;

public:
	WJCELL(const double& h) : height(h) {}
	~WJCELL() = default;

	void initCSS(std::string& sCell, std::string& sRowHeader);
	std::unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);
};

class WJHEADER : public Wt::WItemDelegate
{
	std::string cssColHeader, cssTopLeft;
	const double height;

public:
	WJHEADER(const double& h) : height(h) {}
	~WJHEADER() = default;

	void initCSS(std::string& sColHeader, std::string& sTopLeft);
	std::unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags);
};
