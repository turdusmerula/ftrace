/**
  @file		Timing.cpp
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Time measurement functions.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.
*/

#include "com/Timing.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#define NOMFICH_CPUINFO "/proc/cpuinfo"

using namespace ftrace ;

//-----------------------------------------------------------------------------
double Timing::readCPUFreq()
{
    const char* prefixe_cpu_mhz = "cpu MHz" ;
    FILE* F ;
    char ligne[300+1] ;
    char *pos ;
    double frequence=0 ;

    // Ouvre le fichier
    F = fopen(NOMFICH_CPUINFO, "r") ;
    if (!F)
        return 0 ;

    // Lit une ligne apres l'autre
    while (!feof(F))
    {
        // Lit une ligne de texte
        fgets (ligne, sizeof(ligne), F) ;

        // C'est la ligne contenant la frequence?
        if (!strncmp(ligne, prefixe_cpu_mhz, strlen(prefixe_cpu_mhz)))
        {
            // Oui, alors lit la frequence
            pos = strrchr (ligne, ':') +2 ;
            if (!pos)
                break ;
            if (pos[strlen(pos)-1] == '\n')
                pos[strlen(pos)-1] = '\0' ;
            strcpy (ligne, pos) ;
            strcat (ligne,"e6") ;
            frequence = atof (ligne) ;
            break ;
        }
    }
    fclose (F) ;
    return frequence ;
}

//-----------------------------------------------------------------------------
long double Timing::tickToSecond(uint64_t ticks_)
{
	static long double cpuFreq=readCPUFreq() ;
	//Conversion du temps en s
	return static_cast<long double>(ticks_)/cpuFreq ;
}

//-----------------------------------------------------------------------------
std::string Timing::getDate()
{
    char buffer[256] ;
    time_t timestamp = time(NULL);

    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H-%M-%SZ", localtime(&timestamp));
    //sprintf(buffer, "%s.%dZ", buffer, timestamp.millitm) ;

    return buffer ;
}

//-----------------------------------------------------------------------------
std::string Timing::computeTime(uint64_t _time, TimeEnum _timing)
{
	std::stringstream TmpStream ;

	if(_timing==eSecond)
	{
        long double TimeSec=tickToSecond(_time) ;
		TmpStream << TimeSec ;
	}
	else if(_timing==eAuto)
	{
        long double TimeSec=tickToSecond(_time) ;
		if(TimeSec<0.1e-6)
			TmpStream << TimeSec*1.e9 << "ns" ;
		else if(TimeSec<0.1e-3)
			TmpStream << TimeSec*1.e6 << "us" ;
		else if(TimeSec<0.1)
			TmpStream << TimeSec*1.e3 << "ms" ;
		else if(TimeSec<60.)
			TmpStream <<  TimeSec << "s" ;
		else
		{
			double min, sec ;

			//Récupération des minutes
			min = (static_cast<long long int>(TimeSec))/60 ;
			//Récupération des secondes
			sec = TimeSec-static_cast<long double>(min)*60. ;
			TmpStream << min << "m" << sec << "s" ;
		}
	}
	else
	{
	    TmpStream <<  _time ;
    }

	return TmpStream.str() ;
}
