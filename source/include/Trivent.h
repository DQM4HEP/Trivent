  /// \file Trivent.h
/*
 *
 * Trivent.h header template automatically generated by a class generator
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


#ifndef TRIVENT_H
#define TRIVENT_H

/**
 *
\mainpage Trivent repository on <a href="https://github.com/DQM4HEP/Trivent">GitHub</a>

<b> Trivent : A time clustering algorithm </b> <br/>

<b>Trivent</b> is an algorithm used to split trigger event that contains many other sub-events.
It basically builds a time spectrum of your input event and cluster inputs into sub event structure where time peaks are found.

The base class of the framework is trivent::Trivent that perform the algorithm using its own event structure trivent::Event.
The base unit of an event is called trivent::Unit which contains :
<ul>
	<li> the time stamp of the unit </li>
	<li> the collection name of the unit </li>
	<li> a user input void pointer to map a user input object </li>
</ul>

A <a href="http://lcio.desy.de/">LCIO</a> binding is provided with helper functions to encode/decode the base Trivent Event structure.

Here an example on how to process an LCevent with the RawCalorimeterHit class from LCIO :

@code
	EVENT::LCEvent *pLCEvent = new EVENT::LCEvent();

	// ...
	// fill the event with raw calorimeter hits
	// ...

	// create a Trivent instance and parameters
	trivent::Trivent trivent;
	trivent::Trivent::Parameters parameters;
	parameters.m_timeWindow = 3;

	// initialize trivent
	trivent.init(parameters);

	// add the raw calorimeter hit collection from the LCEvent
	// into the trivent Event structure
	trivent::Event triventEvent;
	trivent::LCTrivent::addCollection<EVENT::RawCalorimeterHit, int, &EVENT::RawCalorimeterHit::getTimeStamp>( pLCEvent , "MyRawHitCollection" , triventEvent );

	// process the event
	trivent.processEvent(triventEvent);
@endcode

*/

// -- std headers
#ifdef __APPLE__
#include <_types.h>
#include <_types/_uint8_t.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int64_t.h>
#else
#include <stdint.h>
#endif
#include <string>
#include <set>
#include <map>

// -- trivent headers
#include "TriventListener.h"
#include "Unit.h"

namespace trivent
{

class Event;

/** 
 *  @brief  Trivent class
 */ 
class Trivent 
{
	typedef std::map<uint64_t, UnitSet>  TimeSpectrum;

public:
	/**
	 *  @brief Parameters class
	 */
	class Parameters
	{
	public:
		/**
		 *  @brief  Constructor
		 */
		Parameters();

	public:
		uint64_t                         m_timeWindow;             ///< The time window around a bin to look
		uint32_t                         m_minPeakSize;            ///< The minimum peak size to consider
		uint32_t                         m_minElements;            ///< The minimum number of unit elements for all collections
		uint32_t                         m_maxElements;            ///< The maximum number of unit elements for all collections

		std::set<std::string>            m_maskCollectionNames;    ///< The collection names to mask during processing
	};

public:
	/**
	 *  @brief  Constructor
	 */
	Trivent();

	/**
	 *  @brief  Destructor
	 */
	virtual ~Trivent();

	/**
	 *  @brief  Initialize the trivent algorithm with parameters
	 *
	 *  @param  parameters the input parameters needed for the Trivent algorithm
	 */
	void init(const Parameters &parameters);

	/**
	 *  @brief  Add a Trivent listener
	 *
	 *  @param  pListener a Trivent listener
	 */
	void addListener(TriventListener *pListener);

	/**
	 *  @brief  Remove a Trivent listener
	 *
	 *  @param  pListener a Trivent listener
	 */
	void removeListener(TriventListener *pListener);

	/**
	 *  @brief  Process a trivent event.
	 *          Split the event in different sub events by
	 *          clustering them within time peaks.
	 *          Each time an event is found, listeners are notified
	 *
	 *  @param  inputEvent a user input event
	 */
	virtual void processEvent(const Event &inputEvent);

private:
	/**
	 *  @brief  Notify listeners of a start of event processing
	 *  
	 *  @param  pInputEvent the input event as provided by the user
	 */
	void notifyStartProcessing(const Event *const pInputEvent);

	/**
	 *  @brief  Build the time spectrum from the input unit set
	 *
	 *  @param  unitSet the input unit set to build the time spectrum
	 *  @param  timeSpectrum the time spectrum to fill
	 */
	void getTimeSpectrum(const UnitSet &inputUnits, TimeSpectrum &timeSpectrum);

	/**
	 *  @brief  Get the initial time of the time spectrum
	 *
	 *  @param  timeSpectrum the time spectrum in which to find the intial time
	 *  @param  initialBinIter the initial bin iterator to receive
	 */
	void getInitialTimeBin(const TimeSpectrum &timeSpectrum, TimeSpectrum::const_iterator &initialBinIter);

	/**
	 *  @brief  Find the next time spectrum peak
	 *
	 *  @param  timeSpectrum the input time spectrum
	 *  @param  spectrumBin the previous time peak found, the next time time peak to receive
	 */
	void findNextSpectrumPeak(const TimeSpectrum &timeSpectrum, TimeSpectrum::const_iterator &spectrumBin);

	/**
	 *  @brief  Build an event at the identified time peak
	 *
	 *  @param  outputEvent the output event to fill
	 *  @param  timeSpectrum the input time spectrum to get units
	 *  @param  spectrumBin the peak bin iterator in time spectrum to build event from
	 */
	void buildEvent(Event &outputEvent, const TimeSpectrum &timeSpectrum, const TimeSpectrum::const_iterator &spectrumBin);

	/**
	 *  @brief  Add a set of units from the peak bin to the event
	 *
	 *  @param  outputEvent the event to fill
	 *  @param  spectrumBin the spectrum peak bin containing the units to fill the event with
	 */
	void addUnitsToEvent(Event &outputEvent, const TimeSpectrum::const_iterator &spectrumBin);

	/**
	 *  @brief  Seek the spectrum bin for the next event to process
	 *
	 *  @param  timeSpectrum the input time spectrum
	 *  @param  spectrumBin the current time peak bin, the next time peak bin to receive
	 */
	void seekBinForNextEvent(const TimeSpectrum &timeSpectrum, TimeSpectrum::const_iterator &spectrumBin);

	/**
	 *  @brief  Notify Trivent listeners that an event as been reconstructed
	 *
	 *  @param  outputEvent the reconstructed event to notify
	 */
	void notifyReconstructedEvent(const Event &outputEvent);

private:
	bool                             m_initialized;            ///< Whether Trivent has been initialized
	TriventListenerSet               m_listeners;              ///< The set of Trivent listeners

	// algorithm parameters
	uint64_t                         m_timeWindow;             ///< The time window around a bin to look
	uint32_t                         m_minPeakSize;            ///< The minimum peak size to consider
	uint32_t                         m_minElements;            ///< The minimum number of unit elements for all collections
	uint32_t                         m_maxElements;            ///< The maximum number of unit elements for all collections
	std::set<std::string>            m_maskCollectionNames;    ///< The collection names to mask during processing
};

} 

#endif  //  TRIVENT_H
