  /// \file lc_trivent.cc
/*
 *
 * lc_trivent.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
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
 * DQM4HEP is distributed in the hope that it will be useful,
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
#include "TriventConfig.h"
#include "LCTrivent.h"
#include "TriventListener.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- lcio headers
#include "IO/LCReader.h"
#include "IO/LCWriter.h"
#include "IOIMPL/LCFactory.h"
#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/RawCalorimeterHit.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/LCIO.h"

// -- std headers
#include <iostream>
#include <limits>
#include <signal.h>

using namespace std;

bool shouldExit = false;
unsigned int verbosityLevel = 1;
#define LC_TRIVENT_LOG(level, message) if(level >= verbosityLevel) std::cout << message;

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int signal)
{
	if(shouldExit)
		exit(0);

	shouldExit = true;
}

//-------------------------------------------------------------------------------------------------

class LCTriventWriter : public trivent::LCTriventListener
{
public:
	LCTriventWriter(const std::string &outputFileName) :
		LCTriventListener(),
		m_nProcessedEvent(0)
	{
		m_pLCWriter = IOIMPL::LCFactory::getInstance()->createLCWriter();
		m_pLCWriter->open( outputFileName , EVENT::LCIO::WRITE_NEW );
	}

	~LCTriventWriter()
	{
		LC_TRIVENT_LOG( 2, "Number of reconstructed events : " << m_nProcessedEvent << std::endl);
		m_pLCWriter->close();
		delete m_pLCWriter;
	}

	void processReconstructedEvent(EVENT::LCEvent *pLCEvent)
	{
		LC_TRIVENT_LOG( 1, "Writing rec event no " << pLCEvent->getEventNumber() << " to disk" << std::endl);

		if(0 == verbosityLevel)
			UTIL::LCTOOLS::dumpEvent(pLCEvent);

		m_pLCWriter->writeEvent(pLCEvent);
		m_nProcessedEvent++;
	}

private:
	unsigned int               m_nProcessedEvent;
	IO::LCWriter              *m_pLCWriter;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', Trivent_VERSION_STR);

	TCLAP::ValueArg<unsigned int> nMaxEventsArg(
				  "n"
				 , "max-events"
				 , "The maximum number of events to process"
				 , false
				  , std::numeric_limits<unsigned int>::max()
				 , "unsigned int");
	pCommandLine->add(nMaxEventsArg);

	TCLAP::ValueArg<std::string> outputFileNameArg(
				  "o"
				 , "output-file"
				 , "The lcio output file to produce"
				 , false
				 , "lc_trivent_output.slcio"
				 , "string");
	pCommandLine->add(outputFileNameArg);

	TCLAP::MultiArg<std::string> inputFileNamesArg(
				  "f"
				 , "lcio-input-files"
				 , "The list of lcio input file to process"
				 , true
				 , "string");
	pCommandLine->add(inputFileNamesArg);

	TCLAP::MultiArg<std::string> collectionNamesArg(
				  "c"
				 , "collection-names"
				 , "The list of lcio collection to process. Allowed types : CalorimeterHit, RawCalorimeterHit"
				 , true
				 , "string");
	pCommandLine->add(collectionNamesArg);

	TCLAP::ValueArg<unsigned int> verbosityArg(
				  "v"
				 , "verbosity"
				 , "The verbosity level (from 0 to 3)"
				 , false
				 , 1
				 , "unsigned int");
	pCommandLine->add(verbosityArg);

	TCLAP::ValueArg<unsigned int> minNElementsArg(
				  "l"
				 , "min-elements"
				 , "The minimum number of trivent unit to process an event"
				 , false
				 , 0
				 , "unsigned int");
	pCommandLine->add(minNElementsArg);

	TCLAP::ValueArg<unsigned int> maxNElementsArg(
				  "u"
				 , "max-elements"
				 , "The maximum number of trivent unit to process an event"
				 , false
				 , std::numeric_limits<unsigned int>::max()
				 , "unsigned int");
	pCommandLine->add(maxNElementsArg);

	TCLAP::ValueArg<unsigned int> peakSizeArg(
				  "p"
				 , "peak-size"
				 , "The minimum peak size to consider a reconstructed event"
				 , false
				 , 10
				 , "unsigned int");
	pCommandLine->add(peakSizeArg);

	TCLAP::ValueArg<unsigned int> timeWindowArg(
				  "w"
				 , "time-window"
				 , "The time window around a local peak in time spectrum"
				 , false
				 , 2
				 , "unsigned int");
	pCommandLine->add(timeWindowArg);

	// parse command line
	LC_TRIVENT_LOG( 2, "lc_trivent : Parsing command line ..." << std::endl);
	pCommandLine->parse(argc, argv);

	verbosityLevel = verbosityArg.getValue();

	// install signal handlers
	LC_TRIVENT_LOG( 2, "Installing signal handler ..." << std::endl);
	signal(SIGINT,  int_key_signal_handler);

	// input stuff
	const std::vector<std::string> lcioInputFiles(inputFileNamesArg.getValue());
	const std::vector<std::string> collectionNames(collectionNamesArg.getValue());
	IO::LCReader *pLCReader = IOIMPL::LCFactory::getInstance()->createLCReader();
	pLCReader->open( lcioInputFiles );

	// output stuff
	const std::string lcioOutputFile(outputFileNameArg.getValue());
	LCTriventWriter *pLCTriventWriter = new LCTriventWriter(lcioOutputFile);

	// trivent stuff
	trivent::Trivent *pTrivent = new trivent::Trivent();

	trivent::Trivent::Parameters triventParameters;
	triventParameters.m_timeWindow = timeWindowArg.getValue();
	triventParameters.m_minPeakSize = peakSizeArg.getValue();
	triventParameters.m_minElements = minNElementsArg.getValue();
	triventParameters.m_maxElements = maxNElementsArg.getValue();
	pTrivent->init(triventParameters);

	pTrivent->addListener(pLCTriventWriter);

	EVENT::LCEvent *pLCEvent(0);

	unsigned int nProcessedEvents(0);
	time_t startTime(time(0));

	LC_TRIVENT_LOG( 2, "Processing ..." <<  std::endl);

	// start proccessing
	while( (pLCEvent = pLCReader->readNextEvent()) )
	{
		nProcessedEvents++;

		if(nMaxEventsArg.getValue() <= nProcessedEvents)
		  {
		    std::cout << "Stopped after processing " << nProcessedEvents << " evts (on usr query)" << std::endl;
			break;
		  }

		if(shouldExit)
		  {
		    std::cout << "Stopped after processing " << nProcessedEvents << " evts (exit flag)" << std::endl;
			break;
		  }

		trivent::Event triventEvent;

		// fill event with specified collections
		for(std::vector<std::string>::const_iterator colIter = collectionNames.begin(), colEndIter = collectionNames.end() ;
				colEndIter != colIter ; ++colIter)
		{
			try
			{
				EVENT::LCCollection *pLCCollection = pLCEvent->getCollection(*colIter);

				if(pLCCollection->getTypeName() == EVENT::LCIO::CALORIMETERHIT)
				{
					trivent::LCTrivent::addCollection<EVENT::CalorimeterHit>(pLCEvent,
							*colIter, triventEvent, &EVENT::CalorimeterHit::getTime);
				}
				else if(pLCCollection->getTypeName() == EVENT::LCIO::RAWCALORIMETERHIT)
				{
					 trivent::LCTrivent::addCollection<EVENT::RawCalorimeterHit>(pLCEvent,
							 *colIter, triventEvent, &EVENT::RawCalorimeterHit::getTimeStamp);
				}
				else
				{
					LC_TRIVENT_LOG( 3, "Couldn't treat collection '" << *colIter << "' of type '" << pLCCollection->getTypeName() << "' ! Skipping ..." << std::endl );
					continue;
				}
			}
			catch(EVENT::DataNotAvailableException &exception)
			{
				LC_TRIVENT_LOG( 3, "Collection '" << *colIter << "' not available. What : " << exception.what() << ". Skipping ..." << std::endl );
				continue;
			}
		}

		triventEvent.setUserEvent( (void *) pLCEvent );
		pTrivent->processEvent(triventEvent);
	}

	time_t endTime(time(0));

	LC_TRIVENT_LOG( 2, "Number of processed events : " << nProcessedEvents << " evts" <<  std::endl);
	LC_TRIVENT_LOG( 2, "Processing time : " << endTime - startTime << " secs" << std::endl);
	LC_TRIVENT_LOG( 2, "Processing rate : " << static_cast<float>(nProcessedEvents) / static_cast<int>(endTime - startTime) << " evts/sec" << std::endl);

	delete pCommandLine;

	pLCReader->close();
	delete pLCTriventWriter;

	delete pTrivent;

	return 0;
}
