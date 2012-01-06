/*
Copyright 2011 Clint Bellanger

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

// This macro should be used instead of the standard cassert, as it does
// cleanup.

#ifdef __PRETTY_FUNCTION__
#define FLARE_FUNCTION __PRETTY_FUNCTION__
#else
#define FLARE_FUNCTION __FUNCTION__
#endif

void AssertionError(char const* exp, char const* file, char const* func, int line);

#ifndef NDEBUG
#define FlareAssert(expr) do { if (expr); else AssertionError(#expr, __FILE__, FLARE_FUNCTION, __LINE__); } while(false)
#else
#define FlareAssert(expr) do {} while(false)
#endif

