  /// \file TriventListener.h
/*
 *
 * TriventListener.h header template automatically generated by a class generator
 * Creation date : ven. mars 18 2016
 *
 * This file is part of Trivent libraries.
 * 
 * Trivent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * Trivent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Trivent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef TRIVENTLISTENER_H
#define TRIVENTLISTENER_H

// -- std headers
#include <set>

namespace trivent
{

class Event;

/**
 *  @brief  TriventListener class
 */
class TriventListener
{
public:
	/**
	 *  @brief  Destructor
	 */
	virtual ~TriventListener() {}

	/**
	 *  @brief  Notify when an user input event is going to be processed
	 *
	 *  @param  the user input event to process
	 */
	virtual void startProcessingInputEvent(const Event *const pInputEvent) = 0;

	/**
	 *  @brief  Call back method to process a reconstructed event by the trivent algorithm
	 *
	 *  @param  pReconstructedEvent a Trivent reconstructed event
	 */
	virtual void processReconstructedEvent(const Event *const pReconstructedEvent) = 0;
};

typedef std::set<TriventListener*>  TriventListenerSet;

}

#endif  //  TRIVENTLISTENER_H
