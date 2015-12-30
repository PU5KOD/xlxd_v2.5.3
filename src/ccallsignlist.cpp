//
//  ccallsignlist.cpp
//  xlxd
//
//  Created by Jean-Luc Deltombe (LX3JL) on 30/12/2015.
//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//
// ----------------------------------------------------------------------------
//    This file is part of xlxd.
//
//    xlxd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    xlxd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "main.h"
#include "ccallsignlist.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CCallsignList::CCallsignList()
{
    m_Filename = NULL;
    ::memset(&m_LastModTime, 0, sizeof(CCallsignList));
}

////////////////////////////////////////////////////////////////////////////////////////
// destructor

CCallsignList::~CCallsignList()
{
    if ( m_Filename != NULL )
    {
        delete m_Filename;
    }
}



////////////////////////////////////////////////////////////////////////////////////////
// file io

bool CCallsignList::LoadFromFile(const char *filename)
{
    bool ok = false;
    char sz[CALLSIGN_LEN+1];
    
    // and load
    std::ifstream file (filename);
    if ( file.is_open() )
    {
        Lock();
        
        // empty list
        clear();
        // fill with file content
        while ( file.getline(sz, sizeof(sz)).good()  )
        {
            push_back(CCallsign(sz));
        }
        // close file
        file.close();
        
        // keep file path
        m_Filename = filename;
        
        // update time
        GetLastModTime(&m_LastModTime);
        
        // and done
        Unlock();
        ok = true;
        std::cout << "Gatekeeper loaded " << size() << " lines from " << filename <<  std::endl;
    }
    else
    {
        std::cout << "Gatekeeper cannot find " << filename <<  std::endl;
    }

    return ok;
}

bool CCallsignList::ReloadFromFile(void)
{
    bool ok = false;
    
    if ( m_Filename !=  NULL )
    {
        ok = LoadFromFile(m_Filename);
    }
    return ok;
}

bool CCallsignList::NeedReload(void)
{
    bool needReload = false;
    
    time_t time;
    if ( GetLastModTime(&time) )
    {
        needReload = time != m_LastModTime;
    }
    return needReload;
}

bool CCallsignList::GetLastModTime(time_t *time)
{
    bool ok = false;
 
    if ( m_Filename != NULL )
    {
        int file=0;
        if( (file = ::open(m_Filename, O_RDONLY)) != -1 )
        {
            struct stat fileStat;
            if( ::fstat(file, &fileStat) != -1 )
            {
                *time = fileStat.st_mtime;
                ok = true;
            }
        }

    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////////////
// compare

bool CCallsignList::IsListed(const CCallsign &callsign) const
{
    bool listed = false;
    
    for ( int i =  0; (i < size()) && !listed; i++ )
    {
        listed = (data()[i]).HasSameCallsignWithWidlcard(callsign);
    }
    
    return listed;
}