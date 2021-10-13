#include "jpdfcell.h"

void JPDFCELL::drawCellBar(HPDF_Page& page, double barWidth, JFUNC& jf)
{
	// Coloured bars take the full cell height, rather than the vBLTR/text height.
	HPDF_STATUS error;
	int index = vBLTR.size() - 1;
	double xCoord = vBLTR[index][0][0];
	double yCoord = cellBLTR[0][1] + padding;
	double barHeight = cellBLTR[1][1] - padding - yCoord;
	for (int ii = 0; ii < vBarColour.size(); ii++)
	{
		error = HPDF_Page_SetRGBFill(page, vBarColour[ii][0], vBarColour[ii][1], vBarColour[ii][2]);
		if (error != HPDF_OK) { jf.err("SetRGBFill-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Rectangle(page, xCoord, yCoord, barWidth, barHeight);
		if (error != HPDF_OK) { jf.err("Rectangle-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Fill(page);
		if (error != HPDF_OK) { jf.err("FillStroke-jpdfcell.drawCellBar"); }
		xCoord += barWidth;
	}
	vector<vector<double>> newBLTR = vBLTR[index];
	vBLTR[index][1][0] = xCoord;
	xCoord += padding;
	newBLTR[0][0] = xCoord;
	vBLTR.push_back(newBLTR);
}
void JPDFCELL::drawCellText(HPDF_Page& page, string text, JFUNC& jf)
{
	string piece1, piece2;
	bool inPieces = 0;
	int index = vBLTR.size() - 1;
	double fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	float availableWidth = vBLTR[index][1][0] - vBLTR[index][0][0];

	float textWidth = HPDF_Page_TextWidth(page, text.c_str());
	if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.drawCellText"); }
	else if (availableWidth < textWidth)
	{
		if (vBLTR[index][0][1] - cellBLTR[0][1] > fontHeight)  // If enough space is available for another line...
		{
			inPieces = 1;
			size_t pos1 = text.size();
			while (availableWidth < textWidth)
			{
				pos1 = text.rfind(' ', pos1 - 1);
				if (pos1 > text.size()) { break; }
				piece1 = text.substr(0, pos1);
				textWidth = HPDF_Page_TextWidth(page, piece1.c_str());
				if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.drawCellText"); }
			}
			if (pos1 < text.size()) { piece2 = text.substr(pos1 + 1); }
			else
			{
				piece1 = "";
				piece2 = text;
			}
		}
		else { jf.err("Insufficient space for text-jpdfcell.drawCellText"); }
	}

	HPDF_STATUS error = HPDF_Page_BeginText(page);
	if (error != HPDF_OK) { jf.err("BeginText-jpdfcell.drawCellText"); }
	double bottomPadding = (vBLTR[index][1][1] - vBLTR[index][0][1] - fontHeight);
	if (!inPieces)
	{
		error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], text.c_str(), HPDF_TALIGN_LEFT, NULL);
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCellText"); }
	}
	else
	{
		error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], piece1.c_str(), HPDF_TALIGN_LEFT, NULL);
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCellText"); }
		vector<vector<double>> newLineBLTR(2, vector<double>(2));
		if (splitPos.size() > 0)
		{
			newLineBLTR[0][0] = splitPos[splitPos.size() - 1] + padding;
		}
		else { newLineBLTR[0][0] = cellBLTR[0][0] + padding; }
		newLineBLTR[1][0] = cellBLTR[1][0] - padding;
		newLineBLTR[0][1] = vBLTR[index][0][1] - fontHeight - (2.0 * padding);
		newLineBLTR[1][1] = vBLTR[index][1][1] - fontHeight - (2.0 * padding);
		vBLTR.push_back(newLineBLTR);
		index++;
		error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], piece2.c_str(), HPDF_TALIGN_LEFT, NULL);
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCellText"); }
		textWidth = HPDF_Page_TextWidth(page, piece2.c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.drawCellText"); }
	}
	error = HPDF_Page_EndText(page);
	if (error != HPDF_OK) { jf.err("EndText-jpdfcell.drawCellText"); }

	vector<vector<double>> newBLTR = vBLTR[index];
	vBLTR[index][1][0] = vBLTR[index][0][0] + textWidth;
	newBLTR[0][0] += textWidth;
	vBLTR.push_back(newBLTR);
}
void JPDFCELL::drawCellTextItalic(HPDF_Page& page, int indexText, JFUNC& jf)
{
	if (vsText.size() <= indexText) { jf.err("Text not initialized-jpdfcell.drawCellTextItalic"); }
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontItalic, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextItalic"); }
	drawCellText(page, vsText[indexText], jf);
}
void JPDFCELL::drawCellTextItalic(HPDF_Page& page, string text, JFUNC& jf)
{
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontItalic, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextItalic"); }
	drawCellText(page, text, jf);
}
void JPDFCELL::drawCellTextPlain(HPDF_Page& page, int indexText, JFUNC& jf)
{
	if (vsText.size() <= indexText)
	{
		string warning = "Made blank entry (row " + to_string(rowIndex);
		warning += ", col " + to_string(colIndex) + ", indexText ";
		warning += to_string(indexText) + ")-jpdfcell.drawCellTextPlain";
		jf.log(warning);
		vsText.resize(indexText + 1);
		vsText[indexText] = " ";
	}
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextPlain"); }
	drawCellText(page, vsText[indexText], jf);
}
void JPDFCELL::drawCellTextPlain(HPDF_Page& page, string text, JFUNC& jf)
{
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextPlain"); }
	drawCellText(page, text, jf);
}
double JPDFCELL::getMaxFontHeight()
{
	int index = vBLTR.size() - 1;
	if (index < 0) { return -1.0; }
	double maxHeight = vBLTR[index][1][1] - vBLTR[index][0][1] - (2.0 * padding);
	return maxHeight;
}
