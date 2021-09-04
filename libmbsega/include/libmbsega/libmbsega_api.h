/*
    This file is part of libmbsega.
    Copyright (C) 2021 BueniaDev.

    libmbsega is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libmbsega is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libmbsega.  If not, see <https://www.gnu.org/licenses/>.
*/

    
#ifndef LIBMBSEGA_API_H
#define LIBMBSEGA_API_H

#if defined(_MSC_VER) && !defined(LIBMBSEGA_STATIC)
    #ifdef LIBMBSEGA_EXPORTS
        #define LIBMBSEGA_API __declspec(dllexport)
    #else
        #define LIBMBSEGA_API __declspec(dllimport)
    #endif // LIBMBSEGA_EXPORTS
#else
    #define LIBMBSEGA_API
#endif // _MSC_VER

#endif // LIBMBSEGA_API_H
