/***************************************************************************
common.hpp  -  description
-------------------
begin                : cs march 14 2004
copyright            : (C) 2004 by Ferenc Bodon
email                : bodon@cs.bme.hu
***************************************************************************/

#ifndef common_HPP
#define common_HPP

/**
*@author Ferenc Bodon
*/

/** The type of the item.
*
* Items are represented by non-negative integers.
* If we know that no itemcode is larger than \f$2^{16}\f$, we can recomplie
* the code using unsigned short as itemtype.
*/
typedef unsigned int itemtype;
//typedef unsigned short itemtype;
//typedef long itemtype;

/** The type of the counter for support counting.
*
* Counters are represented by non-negative integers.
* If we know that the transaction file does not contain more than n \f$2^{16}\f$ transactions, we can recomplie
* the code using unsigned short as type.
*/
typedef unsigned int countertype;

#endif
