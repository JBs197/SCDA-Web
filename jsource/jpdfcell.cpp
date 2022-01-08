#include "jpdfcell.h"

void JPDFCELL::drawCellBar(HPDF_Page& page, JFUNC& jf, double barWidth)
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
		if (error != HPDF_OK) { err("SetRGBFill-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Rectangle(page, xCoord, yCoord, barWidth, barHeight);
		if (error != HPDF_OK) { err("Rectangle-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Fill(page);
		if (error != HPDF_OK) { err("FillStroke-jpdfcell.drawCellBar"); }
		xCoord += barWidth;
	}
	vector<vector<double>> newBLTR = vBLTR[index];
	vBLTR[index][1][0] = xCoord;
	xCoord += padding;
	newBLTR[0][0] = xCoord;
	vBLTR.push_back(newBLTR);
}
void JPDFCELL::drawCellText(HPDF_Page& page, JFUNC& jf, string text, HPDF_Font& Font)
{
	// Uses the entire (unused) cell area to paint the text.
	if (text.size() < 1) { return; }
	int index = vBLTR.size() - 1;
	int lineIndex, maxLineIndex = 0;
	double left = vBLTR[index][0][0];
	double right = vBLTR[index][1][0];
	double availableWidth = right - left;
	double fontHeightMax = vBLTR[index][1][1] - vBLTR[index][0][1];
	double fontSize = fontHeightMax;
	double internalPadding, lineWidth, spaceWidth, textWidth;
	HPDF_STATUS error;
	string temp;
	vector<string> vText = jf.splitByMarker(text, ' ');  // Split by word.

	// Firstly, determine the maximum font size which can be used.
	while (1) {
		error = HPDF_Page_SetFontAndSize(page, Font, fontSize);
		if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellText"); }
		spaceWidth = (double)HPDF_Page_TextWidth(page, " ");
		if (spaceWidth == 0.0) { err("TextWidth-jpdfcell.drawCellText"); }

		lineWidth = 0.0;
		lineIndex = 0;
		for (int ii = 0; ii < vText.size(); ii++) {
			textWidth = (double)HPDF_Page_TextWidth(page, vText[ii].c_str());
			if (textWidth == 0.0) { err("TextWidth-jpdfcell.drawCellText"); }
			if (ii == 0) { lineWidth += textWidth; }
			else { lineWidth += spaceWidth + textWidth; }

			if (lineWidth > availableWidth) {
				if (lineIndex < maxLineIndex) {  // Move onto the cell's next line and proceed.
					lineIndex++;
					lineWidth = textWidth;
				}
				else { break; }  // Failure. Reduce font size and try again.
			}
		}
		if (lineWidth <= availableWidth) { break; }  // Success.
		fontSize -= 1.0;
		internalPadding = (double)(maxLineIndex + 1) * padding;
		if (((double)(maxLineIndex + 1) * fontSize) + internalPadding <= fontHeightMax) {
			maxLineIndex++;  // The font is sufficiently small to permit a new line of text.
		}
	}

	// Resize vBLTR if necessary.
	if (lineIndex < maxLineIndex) {
		vBLTR[index][0][1] += ((double)maxLineIndex * (fontSize + padding));
		vBLTR[index][0][1] -= (0.5 * fontSize);
	}
	else if (maxLineIndex > 0) {
		vBLTR[index][0][1] += ((double)maxLineIndex * (fontSize + padding));
	}

	// Paint the text list, updating vBLTR on the fly.
	lineWidth = 0.0;
	for (int ii = 0; ii < vText.size(); ii++) {
		textWidth = (double)HPDF_Page_TextWidth(page, vText[ii].c_str());
		if (textWidth == 0.0) { err("TextWidth-jpdfcell.drawCellText"); }
		if (ii == 0) { lineWidth += textWidth; }
		else { lineWidth += spaceWidth + textWidth; }

		index = vBLTR.size() - 1;
		vector<vector<double>> newBLTR(2, vector<double>(2));
		if (lineWidth > availableWidth) {
			lineWidth = textWidth;
			newBLTR[0][0] = left;
			newBLTR[1][0] = left + textWidth;
			newBLTR[1][1] = vBLTR[index][0][1] - padding;
			newBLTR[0][1] = newBLTR[1][1] - fontSize;
			vBLTR.push_back(newBLTR);
			drawText(page, jf, vText[ii]);
			newBLTR[0][0] = newBLTR[1][0];
			newBLTR[1][0] = right;
			vBLTR.push_back(newBLTR);
		}
		else if (ii == 0) {
			drawText(page, jf, vText[ii]);
			newBLTR = vBLTR[index];
			vBLTR[index][1][0] = vBLTR[index][0][0] + textWidth;
			newBLTR[0][0] = vBLTR[index][1][0];
			vBLTR.push_back(newBLTR);
		}
		else {
			temp = " " + vText[ii];
			drawText(page, jf, temp);
			newBLTR = vBLTR[index];
			vBLTR[index][1][0] = vBLTR[index][0][0] + spaceWidth + textWidth;
			newBLTR[0][0] = vBLTR[index][1][0];
			vBLTR.push_back(newBLTR);
		}
	}
}
void JPDFCELL::drawCellTextItalic(HPDF_Page& page, JFUNC& jf, string text)
{
	drawCellText(page, jf, text, fontItalic);
}
void JPDFCELL::drawCellTextList(HPDF_Page& page, JFUNC& jf, int italicFreq, string separator)
{
	// Paint the cell's vsText with "separator" between elements. If italicFreq > 0, 
	// then italicize vsText elements once per italicFreq.
	if (vsText.size() < 1) { return; }
	bool sepSplit = 0;
	string sepSegFirst, sepSegLast, temp;
	if (separator.size() >= 1 && separator != " ") {
		sepSplit = 1;
		sepSegFirst = separator + " ";
		sepSegLast = " " + separator;
		separator = " " + separator + " ";
	}
	else if (separator.size() < 1) { separator = " "; }
	size_t pos1, pos2;
	int index = vBLTR.size() - 1;
	int lineIndex, maxLineIndex = 0;
	double left = vBLTR[index][0][0];
	double right = vBLTR[index][1][0];
	double availableWidth = right - left;
	double fontHeightMax = vBLTR[index][1][1] - vBLTR[index][0][1];
	double fontSize = fontHeightMax;
	double internalPadding, lineWidth, sepWidth, sepWidthSeg, spaceWidth, textWidth;
	HPDF_STATUS error;
	vector<vector<string>> vvsText = jf.splitByMarker(vsText, ' ');  // Split by word.

	// Firstly, determine the maximum font size which can be used.
	/*
	while (1) {  
		error = HPDF_Page_SetFontAndSize(page, font, fontSize);
		if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellTextList"); }
		sepWidth = (double)HPDF_Page_TextWidth(page, separator.c_str());
		if (sepWidth == 0.0) { err("TextWidth-jpdfcell.drawCellTextList"); }
		if (sepSplit) {
			sepWidthSeg = (double)HPDF_Page_TextWidth(page, sepSegFirst.c_str());
			if (sepWidthSeg == 0.0) { err("TextWidth-jpdfcell.measureCellTextList"); }
			spaceWidth = sepWidth - sepWidthSeg;
		}
		else { spaceWidth = sepWidth; }

		lineWidth = 0.0;
		lineIndex = 0;
		for (int ii = 0; ii < vvsText.size(); ii++) {
			if (ii > 0) {
				lineWidth += sepWidth;
				if (lineWidth > availableWidth) {
					if (sepSplit) {
						lineWidth += (sepWidthSeg - sepWidth);
						if (lineWidth > availableWidth) {
							if (lineIndex < maxLineIndex) {  // Move onto the cell's next line and proceed.
								lineIndex++;
								lineWidth = sepWidthSeg;
							}
							else { break; }  // Failure. Reduce font size and try again.
						}
					}
					else if (lineIndex < maxLineIndex) {  // Move onto the cell's next line and proceed.
						lineIndex++;
						lineWidth = 0.0;
					}
					else { break; }  // Failure. Reduce font size and try again.
				}
			}

			if (italicFreq > 0 && ii % italicFreq == italicFreq - 1) {
				error = HPDF_Page_SetFontAndSize(page, fontItalic, fontSize);
				if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellTextList"); }
			}
			for (int jj = 0; jj < vvsText[ii].size(); jj++) {
				textWidth = (double)HPDF_Page_TextWidth(page, vvsText[ii][jj].c_str());
				if (textWidth == 0.0) { err("TextWidth-jpdfcell.drawCellTextList"); }
				if (jj == 0) { lineWidth += textWidth; }
				else { lineWidth += spaceWidth + textWidth; }
				
				if (lineWidth > availableWidth) {
					if (lineIndex < maxLineIndex) {  // Move onto the cell's next line and proceed.
						lineIndex++;
						lineWidth = textWidth;
					}
					else { break; }  // Failure. Reduce font size and try again.
				}
			}
			if (italicFreq > 0 && ii % italicFreq == italicFreq - 1) {
				error = HPDF_Page_SetFontAndSize(page, font, fontSize);
				if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellTextList"); }
			}
		}
		if (lineWidth <= availableWidth) { break; }  // Success.
		fontSize -= 1.0;
		internalPadding = (double)(maxLineIndex + 1) * padding;
		if (((double)(maxLineIndex + 1) * fontSize) + internalPadding <= fontHeightMax) {
			maxLineIndex++;  // The font is sufficiently small to permit a new line of text.
		}
	}
	
	// Resize vBLTR if necessary.
	if (lineIndex < maxLineIndex) { 
		vBLTR[index][0][1] += ((double)maxLineIndex * (fontSize + padding));
		vBLTR[index][0][1] -= (0.5 * fontSize);
	}
	else if (maxLineIndex > 0) {
		vBLTR[index][0][1] += ((double)maxLineIndex * (fontSize + padding));
	}
	*/

	error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellTextList"); }
	sepWidth = (double)HPDF_Page_TextWidth(page, separator.c_str());
	if (sepWidth == 0.0) { err("TextWidth-jpdfcell.drawCellTextList"); }
	if (sepSplit) {
		sepWidthSeg = (double)HPDF_Page_TextWidth(page, sepSegFirst.c_str());
		if (sepWidthSeg == 0.0) { err("TextWidth-jpdfcell.measureCellTextList"); }
		spaceWidth = sepWidth - sepWidthSeg;
	}
	else { spaceWidth = sepWidth; }

	// Paint the text list, updating vBLTR on the fly.
	lineWidth = 0.0;
	for (int ii = 0; ii < vvsText.size(); ii++) {
		if (ii > 0) {
			lineWidth += sepWidth;
			if (lineWidth > availableWidth) {
				if (sepSplit) {
					lineWidth += (sepWidthSeg - sepWidth);
					if (lineWidth <= availableWidth) {
						lineWidth = 0.0;
						drawText(page, jf, sepSegLast);
						index = vBLTR.size() - 1;
						vector<vector<double>> newBLTR(2, vector<double>(2));
						newBLTR[0][0] = left;
						newBLTR[1][0] = right;
						newBLTR[1][1] = vBLTR[index][0][1] - padding;
						newBLTR[0][1] = newBLTR[1][1] - fontSize;
						vBLTR.push_back(newBLTR);
					}
					else {
						lineWidth = sepWidthSeg;
						index = vBLTR.size() - 1;
						vector<vector<double>> newBLTR(2, vector<double>(2));
						newBLTR[0][0] = left;
						newBLTR[1][0] = left + sepWidthSeg;
						newBLTR[1][1] = vBLTR[index][0][1] - padding;
						newBLTR[0][1] = newBLTR[1][1] - fontSize;
						vBLTR.push_back(newBLTR);
						drawText(page, jf, sepSegFirst);
						newBLTR[0][0] = newBLTR[1][0];
						newBLTR[1][0] = right;
						vBLTR.push_back(newBLTR);
					}
				}
				else { 
					lineWidth = 0.0;	
					index = vBLTR.size() - 1;
					vector<vector<double>> newBLTR(2, vector<double>(2));
					newBLTR[0][0] = left;
					newBLTR[1][0] = right;
					newBLTR[1][1] = vBLTR[index][0][1] - padding;
					newBLTR[0][1] = newBLTR[1][1] - fontSize;
					vBLTR.push_back(newBLTR);
				}
			}
			else {
				drawText(page, jf, separator);
				index = vBLTR.size() - 1;
				vector<vector<double>> newBLTR = vBLTR[index];
				vBLTR[index][1][0] = vBLTR[index][0][0] + sepWidth;
				newBLTR[0][0] = vBLTR[index][1][0];
				vBLTR.push_back(newBLTR);
			}
		}
		if (italicFreq > 0 && ii % italicFreq == italicFreq - 1) {
			error = HPDF_Page_SetFontAndSize(page, fontItalic, fontSize);
			if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellTextList"); }
		}
		for (int jj = 0; jj < vvsText[ii].size(); jj++) {
			textWidth = (double)HPDF_Page_TextWidth(page, vvsText[ii][jj].c_str());
			if (textWidth == 0.0) { err("TextWidth-jpdfcell.drawCellTextList"); }
			if (jj == 0) { lineWidth += textWidth; }
			else { lineWidth += spaceWidth + textWidth; }

			index = vBLTR.size() - 1;
			vector<vector<double>> newBLTR(2, vector<double>(2));
			if (lineWidth > availableWidth) {
				lineWidth = textWidth;
				newBLTR[0][0] = left;
				newBLTR[1][0] = ceil(left + textWidth);
				newBLTR[1][1] = vBLTR[index][0][1] - padding;
				newBLTR[0][1] = newBLTR[1][1] - fontSize;
				vBLTR.push_back(newBLTR);
				drawText(page, jf, vvsText[ii][jj]);
				newBLTR[0][0] = newBLTR[1][0];
				newBLTR[1][0] = right;
				vBLTR.push_back(newBLTR);
			}
			else if (jj == 0) {
				drawText(page, jf, vvsText[ii][jj]);
				newBLTR = vBLTR[index];
				vBLTR[index][1][0] = vBLTR[index][0][0] + textWidth;
				newBLTR[0][0] = vBLTR[index][1][0];
				vBLTR.push_back(newBLTR);
			}
			else {
				temp = " " + vvsText[ii][jj];
				drawText(page, jf, temp);
				newBLTR = vBLTR[index];
				vBLTR[index][1][0] = vBLTR[index][0][0] + spaceWidth + textWidth;
				newBLTR[0][0] = vBLTR[index][1][0];
				vBLTR.push_back(newBLTR);
			}
		}
		if (italicFreq > 0 && ii % italicFreq == italicFreq - 1) {
			error = HPDF_Page_SetFontAndSize(page, font, fontSize);
			if (error != HPDF_OK) { err("SetFontAndSize-jpdfcell.drawCellTextList"); }
		}
	}
}
void JPDFCELL::drawCellTextPlain(HPDF_Page& page, JFUNC& jf, string text)
{
	drawCellText(page, jf, text, font);
}
void JPDFCELL::drawText(HPDF_Page& page, JFUNC& jf, string text)
{
	// Barebones function - assumes measurements and font is already prepared.
	int index = vBLTR.size() - 1;
	HPDF_STATUS error = HPDF_Page_BeginText(page);
	if (error != HPDF_OK) { err("BeginText-jpdfcell.drawText"); }
	error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], text.c_str(), HPDF_TALIGN_LEFT, NULL);
	if (error != HPDF_OK) { err("TextOut-jpdfcell.drawText"); }
	error = HPDF_Page_EndText(page);
	if (error != HPDF_OK) { err("EndText-jpdfcell.drawText"); }
}
void JPDFCELL::err(string message)
{
	string errorMessage = "JPDFCELL error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
double JPDFCELL::getMaxFontHeight()
{
	int index = vBLTR.size() - 1;
	if (index < 0) { return -1.0; }
	double maxHeight = vBLTR[index][1][1] - vBLTR[index][0][1] - (2.0 * padding);
	return maxHeight;
}
