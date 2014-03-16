/**
  @file		Logger.h
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Logger for statistics files.

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

#ifndef LOGGER_H
#define LOGGER_H

#include "scope/Scope.h"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <ostream>

class ThreadData ;

/**
* Column description for logger.
*/
struct Column
{
    /**
    * Enumeration for columns to display in logger.
    */
    typedef enum {  eCall,
                    eAddress ,
                    eSource,
                    eName, eTreeName,
                    eTotalTime, eAvgTime,              //Total time elapsed for scope
                    eTotalScopeTime, eAvgScopeTime,    //Total time in scope only
                    eInstTotalTime, eInstAvgTime,      //Total time in instrument
                    eInstTotalScopeTime, eInstAvgScopeTime, //Total time in instrument for scope
                    eRealTotalTime, eRealAvgTime,      //Real total time elapsed for scope (total time without time elapsed in instrument)
                    eAllocSize,                         //Total amount of memory allocated
                    eAllocNum,                          //Number of memory blocks allocated
                    eTotalAllocTime,                    //Total Time elapsed in alloc
                    eFreeNum,                           //Number of blocs freeds
                    eAllocSource,                       //Source scopes of freed memory
                    eError
                } ColEnum ;


    /**
    * Column type.
    */
    ColEnum _type ;

    /**
    * Max width of data inside column.
    */
    unsigned int _width ;

    /**
    * Title of the column.
    */
    std::string _title ;

    /**
    * Lines of data inside the column.
    */
    std::vector<std::string> _lines ;
} ;

/**
* Patterns available for filtering inside the logger. Patterns are applicable only to name columns.
*/
struct Pattern
{
    /**
    * Indicates if pattern is an include or exclude pattern.
    */
    bool _ignore ;

    /**
    * Pattern to apply.
    */
    boost::regex _pattern ;
} ;

/**
* Logger description.
*/
class Logger
{
public:

    /**
    * Enumeration for display of time in logger.
    */
    typedef enum { eAuto, eSecond, eTicks } TimeEnum ;


    /**
     * Indicates if logger is a root logger.
     * Root loggers does log on console but does not appear in output stat files.
     */
    bool _root ;

    /**
    * Indicates if logger is a tree log.
    */
    bool _treeLog ;

    /**
    * Way of displaying time inside logger.
    */
    TimeEnum _timing ;

    /**
    * Columns to display inside logger.
    */
    std::vector<Column*> _columns ;

    /**
    * Patterns used to filter display data.
    */
    std::vector<Pattern*> _pattern ;

    /**
    * Root scope.
    */
    Scope* _rootScope ;

    /**
    * Root logger. The root logger contains all scopes for the thread.
    */
    Logger* _rootLogger ;

    /**
     * Thread identifier.
     */
    ThreadData* _threadData ;


    /**
    * List of loggers to display.
    */
    static std::list<Logger*>* _loggers ;

    /**
    * Current root logger for the thread.
    */
    static __thread Logger* _threadRootLogger ;


    /**
    * Indicates wether instruments trace each call to standard output
    */
    static __thread bool _threadLogMessagesFlag ;


    /**
    * Internal tabs counter for logger display.
    */
    static __thread int _threadLogTabs ;

    /**
    * Buffer indicating existence of child for displaying tree.
    */
    static __thread std::vector<bool>* _threadScopeChildExist ;

    /**
    * Test if a name must be filtered.
    * @name_ string to be filtered.
    * @logger_ Logger containing the list of patterns to test.
    * @return true if name must be filtered, false else.
    */
    static bool isFiltered(const std::string& name_, Logger* logger_)
        __attribute__((no_instrument_function)) ;

    /**
    * Write the entire stats to a file.
    */
    static void logStats()
        __attribute__((no_instrument_function)) ;

    /**
    * Add a column to a logger.
    * @param type_ type of the column to add.
    * @param logger_ Destination logger.
    */
    static void addColumn(Column::ColEnum type_, Logger* logger_)
        __attribute__((no_instrument_function)) ;

    static void printTabs(int tabNum_)
         __attribute__((no_instrument_function)) ;
    static void printEntryExit(bool entry_)
         __attribute__((no_instrument_function)) ;
    static void printTree(Scope* data_)
         __attribute__((no_instrument_function)) ;

private:
    /**
    * Recursivity depth for tree logging.
    */
    static __thread int _threadDepth ;


    /**
    * Log titles.
    * @param logger_ Logger containing data.
    * @param fout_ Stream to log to.
    */
    static void logColumns(Logger* logger_, std::ostream &fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Log globals scopes to a stream
    * @param logger_ Logger containing data.
    * @param thread_ Thread containing functions to log.
    * @param fout_ Stream to log to.
    */
    static void logGlobalScopes(Logger* logger_, ThreadData* thread_, std::ostream &fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Log the scope data to output stream.
    * @param logger_ Logger containing data.
    * @param scope_ Scope to output.
    * @param fout_ Stream to output to.
    */
    static void logScope(Logger* logger_, Scope* scope_, std::ostream& fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Log globals scopes to a stream
    * @param logger_ Logger containing data.
    * @param fout_ Stream to log to.
    */
    static void logTreeScopes(Logger* logger_, std::ostream &fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Create the tabulation string.
    * @return Result string.
    */
    static std::string computeTab()
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively log the scopes.
    * @param logger_ Logger containing data.
    * @param scope_ Scope to output.
    * @param fout_ Stream to output to.
    */
    static void recusiveLogScopes(Logger* logger_, Scope* scope_, std::ostream& fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Log memory stats of owner scopes for freed blocks.
    * @param logger_ Logger containing data.
    * @param scope_ Scope to output.
    * @param fout_ Stream to output to.
    */
    static void logOwnerBlocks(Logger* logger_, Scope* scope_, std::ostream& fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Log the memory stats for scope.
    * @param logger_ Logger containing data.
    * @param scope_ Scope to output.
    * @param stats_ Memory stats for scope.
    * @param fout_ Stream to output to.
    */
    static void logOwnerBlock(Logger* logger_, Scope* scope_, Scope::MemStats* stats_, std::ostream& fout_)
        __attribute__((no_instrument_function)) ;

} ;



#endif
