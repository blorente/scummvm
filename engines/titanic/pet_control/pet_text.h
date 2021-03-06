/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TITANIC_PET_TEXT_H
#define TITANIC_PET_TEXT_H

#include "common/keyboard.h"
#include "titanic/support/simple_file.h"
#include "titanic/support/screen_manager.h"
#include "titanic/support/text_cursor.h"

namespace Titanic {

class CPetText {
	struct ArrayEntry {
		CString _line;
		CString _rgb;
		CString _string3;
	};
private:
	Common::Array<ArrayEntry> _array;
	CString _lines;
	bool _stringsMerged;
	Rect _bounds;
	int _maxCharsPerLine;
	int _lineCount;
	int _linesStart;
	int _field3C;
	int _field40;
	int _field44;
	int _backR;
	int _backG;
	int _backB;
	int _textR;
	int _textG;
	int _textB;
	int _fontNumber;
	int _field64;
	int _field68;
	int _field6C;
	bool _hasBorder;
	int _scrollTop;
	CTextCursor *_textCursor;
	int _field7C;
private:
	void setupArrays(int count);

	void freeArrays();

	/**
	 * Merges the strings in the strings array
	 */
	void mergeStrings();

	/**
	 * Append text to the current text line
	 */
	void appendText(const CString &str);

	void updateStr3(int lineNum);

	/**
	 * Ensures the Y scrolling for the text is in the valid range
	 */
	void constrainScrollUp(CScreenManager *screenManager);

	/**
	 * Ensures the Y scrolling for the text is in the valid range
	 */
	void constrainScrollDown(CScreenManager *screenManager);

	/**
	 * Get the page height for paging up and down
	 */
	int getPageHeight(CScreenManager *screenManager);
public:
	CPetText(uint count = 10);

	/**
	 * Set up the control
	 */
	void setup();

	/**
	 * Load the data for the control
	 */
	void load(SimpleFile *file, int param);

	/**
	 * Save the data for the control
	 */
	void save(SimpleFile *file, int indent);

	/**
	 * Set the bounds for the control
	 */
	void setBounds(const Rect &bounds) { _bounds = bounds; }

	/**
	 * Sets the flag for whether to draw a frame border around the control
	 */
	void setHasBorder(bool val) { _hasBorder = val; }

	/**
	 * Draw the control
	 */
	void draw(CScreenManager *screenManager);

	void resize(uint count);

	/**
	 * Returns the text from all the lines as a single string
	 */
	CString getText() const;

	/**
	 * Set the text
	 */
	void setText(const CString &str);

	/**
	 * Set text color
	 */
	void setColor(uint col);

	/**
	 * Set text color
	 */
	void setColor(byte r, byte g, byte b);

	/**
	 * Set the color for a line
	 */
	void setLineColor(uint lineNum, byte r, byte g, byte b);

	/**
	 * Set the color for a line
	 */
	void setLineColor(uint lineNum, uint col);

	/**
	 * Sets the maximum number of characters per line
	 */
	void setMaxCharsPerLine(int maxChars);

	/**
	 * Delete the last character from the last line
	 */
	void deleteLastChar();

	void setNPC(int val1, int npcId);

	/**
	 * Get the index into _lines where on-screen text starts
	 */
	int getLinesStart() const { return _linesStart; }

	/**
	 * Scroll the text up
	 */
	void scrollUp(CScreenManager *screenManager);

	/**
	 * Scroll the text down
	 */
	void scrollDown(CScreenManager *screenManager);

	/**
	 * Scroll the text up one page
	 */
	void scrollUpPage(CScreenManager *screenManager);

	/**
	 * Scroll the text down one page
	 */
	void scrollDownPage(CScreenManager *screenManager);

	/**
	 * Scroll to the top of the text
	 */
	void scrollToTop(CScreenManager *screenManager);

	/**
	 * Scroll to the bottom of the text
	 */
	void scrollToBottom(CScreenManager *screenManager);

	/**
	 * Add a line to the text
	 */
	void addLine(const CString &str);

	/**
	 * Add a line to the text
	 */
	void addLine(const CString &str, uint color);

	/**
	 * Add a line to the text
	 */
	void addLine(const CString &str, byte r, byte g, byte b);

	/**
	 * Handles character processing to add or remove characters to
	 * the current text line
	 * @returns		True if the Enter key was pressed
	 */
	bool handleKey(char c);

	/**
	 * Attaches the current system cursor to the text control,
	 * and give it suitable defaults
	 */
	void showCursor(int mode);

	/**
	 * Removes the cursor attached to the text
	 */
	void hideCursor();

	/**
	 * Get an NPC Number embedded within on-screen text.
	 * Used by the PET log to encode which NPC spoke
	 */
	int getNPCNum(uint npcId, uint startIndex);

	/**
	 * Replaces any occurances of line colors that appear in the
	 * first list with the entry at the same index in the dest list
	 */
	void remapColors(uint count, uint *srcColors, uint *destColors);

	/**
	 * Set the font number to use
	 */
	void setFontNumber(int fontNumber);

	/**
	 * Get the width of the text
	 */
	int getTextWidth(CScreenManager *screenManager);

	/**
	 * Get the required height to draw the text
	 */
	int getTextHeight(CScreenManager *screenManager);
};

} // End of namespace Titanic

#endif /* TITANIC_PET_TEXT_H */
