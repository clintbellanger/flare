/*
Copyright © 2011-2012 Clint Bellanger

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class ItemStorage
 */

#ifndef ITEM_STORAGE_H
#define ITEM_STORAGE_H

#include "ItemManager.h"
#include <SDL.h>


class ItemStorage : private Uncopyable {
protected:
	ItemManager *items;
	int slot_number;

public:
	void init(int _slot_number, ItemManager &_items);
	~ItemStorage();

	const ItemStack &operator [] (int slot) const	{return storage[slot];}
	ItemStack &operator [] (int slot)				{return storage[slot];}

	void setItems(std::string s);
	void setQuantities(std::string s);
	std::string getItems();
	std::string getQuantities();
	void add(const ItemStack &stack, int slot = -1);
	void substract(int slot, int quantity = 1);
	bool remove(const Item &item);
	void sort();
	void clear();

	bool full() const;
	int count(const Item &item) const;
	bool contain(const Item &item) const;

	ItemStack * storage;
};

#endif

