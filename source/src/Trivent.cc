  /// \file Trivent.cc
/*
 *
 * Trivent.cc source template automatically generated by a class generator
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

// -- trivent headers
#include "Trivent.h"
#include "Event.h"

// -- std headers
#include <limits>
#include <iostream>
#include <iterator>

namespace trivent
{

Trivent::Parameters::Parameters() :
		m_timeWindow(2),
		m_minPeakSize(10),
		m_minElements(0),
		m_maxElements(std::numeric_limits<uint32_t>::max())
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

Trivent::Trivent() :
		m_initialized(false),
		m_timeWindow(2),
		m_minPeakSize(10)
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

Trivent::~Trivent() 
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

void Trivent::init(const Trivent::Parameters &parameters)
{
	if(m_initialized)
		return;

	m_timeWindow = parameters.m_timeWindow;
	m_minPeakSize = parameters.m_minPeakSize;
	m_minElements = parameters.m_minElements;
	m_maxElements = parameters.m_maxElements;
	m_maskCollectionNames = parameters.m_maskCollectionNames;

	m_initialized = true;
}

//-------------------------------------------------------------------------------------------------

void Trivent::addListener(TriventListener *pListener)
{
	if(NULL == pListener)
		return;

	m_listeners.insert(pListener);
}

//-------------------------------------------------------------------------------------------------

void Trivent::removeListener(TriventListener *pListener)
{
	if(NULL == pListener)
		return;

	m_listeners.erase(pListener);
}

//-------------------------------------------------------------------------------------------------

void Trivent::processEvent(const Event &inputEvent)
{
	// check for initialization
	if( ! m_initialized )
		return;

	// if no listeners, no need to process
	if( m_listeners.empty() )
		return;

	UnitSet allUnits;
	std::vector<std::string> collectionNames = inputEvent.getCollectionNames();

	for(std::vector<std::string>::iterator colIter = collectionNames.begin(), colEndIter = collectionNames.end() ;
			colEndIter != colIter ; ++colIter)
	{
		if( m_maskCollectionNames.find(*colIter) != m_maskCollectionNames.end() )
			continue;

		inputEvent.getUnits(*colIter, allUnits);
	}

	if( allUnits.empty() )
		return;

	this->notifyStartProcessing(&inputEvent);

	if( allUnits.size() < m_minElements || allUnits.size() > m_maxElements )
	{
		std::string unitStr = allUnits.size() < m_minElements ? "Not enough " : "Too much ";
		std::cout << "[Trivent] " << unitStr << "to process an event ! Skipping event !" << std::endl;
		return;
	}

	TimeSpectrum timeSpectrum;

	// get time spectrum from basic units
	this->getTimeSpectrum(allUnits, timeSpectrum);

	// find initial time bin
	TimeSpectrum::const_iterator initialBinIter = timeSpectrum.end();
	this->getInitialTimeBin(timeSpectrum, initialBinIter);

	if( initialBinIter == timeSpectrum.end() )
	{
		std::cout << "[Trivent] No initial time spectrum bin found ! Skipping event !" <<  std::endl;
		return;
	}

	TimeSpectrum::const_iterator spectrumBin = initialBinIter;

	// navigate along the time spectrum and find time peaks
	while( 1 )
	{
		// look for next time peak
		this->findNextSpectrumPeak(timeSpectrum, spectrumBin);

		if( spectrumBin == timeSpectrum.end() )
			break;

		// build an event around the time peak
		Event outputEvent;
		this->buildEvent(outputEvent, timeSpectrum, spectrumBin);

		outputEvent.setUserEvent( inputEvent.getUserEvent() );
		outputEvent.setTimeStamp( spectrumBin->first );

		// notify listeners that an event has been reconstructed
		this->notifyReconstructedEvent(outputEvent);

		// seek the time spectrum iterator to next possible time peak
		this->seekBinForNextEvent(timeSpectrum, spectrumBin);

		// clean the event without deleting trivent units
		outputEvent.clear(false);

		if( spectrumBin == timeSpectrum.end() )
			break;
	}
}

//-------------------------------------------------------------------------------------------------

void Trivent::notifyStartProcessing(const Event *const pInputEvent)
{
	for(TriventListenerSet::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
			endIter != iter ; ++iter)
		(*iter)->startProcessingInputEvent(pInputEvent);
}

//-------------------------------------------------------------------------------------------------

void Trivent::getTimeSpectrum(const UnitSet &inputUnits, Trivent::TimeSpectrum &timeSpectrum)
{
	for(UnitSet::const_iterator iter = inputUnits.begin(), endIter = inputUnits.end() ;
			endIter != iter ; ++iter)
	{
		uint64_t time = (*iter)->getTime();
		timeSpectrum[time].insert(*iter);
	}
}

//-------------------------------------------------------------------------------------------------

void Trivent::getInitialTimeBin(const Trivent::TimeSpectrum &timeSpectrum, Trivent::TimeSpectrum::const_iterator &initialBinIter)
{
	initialBinIter = timeSpectrum.end();

	for(TimeSpectrum::const_iterator iter = timeSpectrum.begin(), endIter = timeSpectrum.end() ;
			endIter != iter ; ++iter)
	{
		if(iter->first >= m_timeWindow)
		{
			initialBinIter = iter;
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------

void Trivent::findNextSpectrumPeak(const Trivent::TimeSpectrum &timeSpectrum, Trivent::TimeSpectrum::const_iterator &spectrumBin)
{
	if( spectrumBin == timeSpectrum.end() )
		return;

	Trivent::TimeSpectrum::const_iterator initialPeakBin = spectrumBin;
	Trivent::TimeSpectrum::const_iterator nextPeakBin = initialPeakBin;

	while( 1 )
	{
		if(nextPeakBin == timeSpectrum.end())
			break;

		if( nextPeakBin->second.size() < m_minPeakSize )
		{
			nextPeakBin++;
			continue;
		}

		uint64_t currentTime = nextPeakBin->first;
		Trivent::TimeSpectrum::const_iterator navigationIter = nextPeakBin;
		navigationIter++;

		// no more data ?
		if( navigationIter == timeSpectrum.end() )
			break;

		bool peakFound = true;

		while( 1 )
		{
			// no further peak found ? Then it's a peak !!!!!
			if( navigationIter == timeSpectrum.end())
			{
				peakFound = true;
				break;
			}

			if( navigationIter->first - currentTime < m_timeWindow && navigationIter->second.size() >= m_minPeakSize )
			{
				peakFound = false;
				break;
			}

			if( navigationIter->first - currentTime > m_timeWindow )
				break;

			navigationIter++;
		}

		if( ! peakFound )
		{
			nextPeakBin++;
			continue;
		}
		else
			break;
	}

	spectrumBin = nextPeakBin;
}

//-------------------------------------------------------------------------------------------------

void Trivent::buildEvent(Event &outputEvent, const TimeSpectrum &timeSpectrum, const TimeSpectrum::const_iterator &spectrumBin)
{
	Trivent::TimeSpectrum::const_iterator navigationBin = spectrumBin;

	// look in previous bins within time window
	if( navigationBin != timeSpectrum.begin() )
	{
		navigationBin --;

		while( 1 )
		{
			// outside time window ?
			if( spectrumBin->first - navigationBin->first > m_timeWindow )
				break;

			this->addUnitsToEvent(outputEvent, navigationBin);

			if( navigationBin == timeSpectrum.begin() )
				break;
			else
				navigationBin --;
		}
	}

	navigationBin = spectrumBin;
	navigationBin ++;

	// look in next bins within time window
	if( navigationBin != timeSpectrum.end() )
	{
		while( 1 )
		{
			if( navigationBin == timeSpectrum.begin() )
				break;

			// outside time window ?
			if( navigationBin->first - spectrumBin->first > m_timeWindow )
				break;

			this->addUnitsToEvent(outputEvent, navigationBin);

			navigationBin ++;
		}
	}

	// populate with current peak bin
	this->addUnitsToEvent(outputEvent, spectrumBin);

}

//-------------------------------------------------------------------------------------------------

void Trivent::addUnitsToEvent(Event &outputEvent, const TimeSpectrum::const_iterator &spectrumBin)
{
	for(UnitSet::const_iterator iter = spectrumBin->second.begin(), endIter = spectrumBin->second.end() ;
			endIter != iter ; ++iter)
		outputEvent.addUnit( (*iter)->getCollectionName(), *iter );
}

//-------------------------------------------------------------------------------------------------

void Trivent::seekBinForNextEvent(const TimeSpectrum &timeSpectrum, TimeSpectrum::const_iterator &spectrumBin)
{
	if(spectrumBin == timeSpectrum.end())
		return;

	uint64_t currentTime = spectrumBin->first;
	spectrumBin ++;

	while( spectrumBin != timeSpectrum.end() && currentTime + m_timeWindow > spectrumBin->first )
	{
		spectrumBin ++;
	}
}

//-------------------------------------------------------------------------------------------------

void Trivent::notifyReconstructedEvent(const Event &outputEvent)
{
	for(TriventListenerSet::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
			endIter != iter ; ++iter)
		(*iter)->processReconstructedEvent(&outputEvent);
}

} 

