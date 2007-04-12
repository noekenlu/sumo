/****************************************************************************/
/// @file    GUIAbstractLoadThread.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class describing the thread that performs the loading of a simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXEventQue.h>
#include <sumo_only/SUMOFrame.h>
#include <utils/common/MsgRetrievingFunction.h>
#include "GUIAbstractLoadThread.h"
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>

#include <ctime>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace FXEX;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIAbstractLoadThread::GUIAbstractLoadThread(MFXInterThreadEventClient *mw,
        MFXEventQue &eq, FXEX::FXThreadEvent &ev)
        : FXSingleEventThread(gFXApp, mw), myParent(mw), myEventQue(eq),
        myEventThrow(ev)
{
    myErrorRetriever = new MsgRetrievingFunction<GUIAbstractLoadThread>(this,
                       &GUIAbstractLoadThread::retrieveError);
    myMessageRetriever = new MsgRetrievingFunction<GUIAbstractLoadThread>(this,
                         &GUIAbstractLoadThread::retrieveMessage);
    myWarningRetreiver = new MsgRetrievingFunction<GUIAbstractLoadThread>(this,
                         &GUIAbstractLoadThread::retrieveWarning);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
}


GUIAbstractLoadThread::~GUIAbstractLoadThread()
{
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetreiver;
}


void
GUIAbstractLoadThread::load(const std::string &file,
                            bool isNet)
{
    _file = file;
    myLoadNet = isNet;
    start();
}


void
GUIAbstractLoadThread::retrieveMessage(const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_MESSAGE, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GUIAbstractLoadThread::retrieveWarning(const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_WARNING, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GUIAbstractLoadThread::retrieveError(const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_ERROR, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


const std::string &
GUIAbstractLoadThread::getFileName() const
{
    return _file;
}



/****************************************************************************/

