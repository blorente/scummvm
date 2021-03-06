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

#ifndef TITANIC_TITANIA_H
#define TITANIC_TITANIA_H

#include "titanic/npcs/character.h"

namespace Titanic {

class CTitania : public CCharacter {
private:
	int _fieldD4;
	int _fieldD8;
	int _fieldDC;
	int _fieldE0;
	int _fieldE4;
	int _fieldE8;
	int _fieldEC;
	int _fieldF0;
	int _fieldF4;
	int _fieldF8;
	int _fieldFC;
	int _field100;
public:
	CLASSDEF;
	CTitania();

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent);

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);
};

} // End of namespace Titanic

#endif /* TITANIC_TITANIA_H */
