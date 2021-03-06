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

#include <scope/Scope.h>
#include <com/Timing.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <ostream>

namespace ftrace {

class ThreadData ;

/**
* Column description for logger.
*/
struct Column
{
    /**
    * Enumeration for columns to display in logger.
    */
    typedef enum {
                    eName,          // demangled name if available
                    eSource,        // source name, not demangled
                    eAddress ,      // function address
                    eParentId,      // parent scope id
                    eId,            // unique id of the scope
                    eCall,          // total number of calls
                    eTime,
                    eError
	} ColEnum ;

	typedef enum {
		eScope,
		eThread,
		eProcess
	} ScopeEnum ;

	/**
    * Column type.
    */
    ColEnum type_ ;

    /**
    * Datan scope.
    */
    ScopeEnum scope_ ;

    /**
    * Max width of data inside column.
    */
    unsigned int width_ ;

    /**
    * Title of the column.
    */
    std::string title_ ;

    /**
    * Lines of data inside the column.
    */
    std::vector<std::string> lines_ ;
} ;

/**
* Patterns available for filtering inside the logger. Patterns are applicable only to name columns.
*/
struct Pattern
{
    /**
    * Indicates if pattern is an include or exclude pattern.
    */
    bool ignore_ ;

    /**
    * Pattern to apply.
    */
    boost::regex pattern_ ;
} ;

/**
* Logger description.
*/
class Logger
{
public:
	typedef enum {
		eText,
		eJson
	} FormatEnum ;


    Logger() ;

    /**
     * Indicates if logger is a root logger.
     * Root loggers does log on console but does not appear in output stat files.
     */
    bool root_ ;

    /**
    * Way of displaying time inside logger.
    */
    Timing::TimeEnum timing_ ;

    /**
    * Columns to display inside logger.
    */
    std::vector<Column*> columns_ ;

    /**
    * Patterns used to filter display data.
    */
    std::vector<Pattern*> pattern_ ;


    /**
    * List of loggers to display.
    */
    static std::list<Logger*>* loggers_ ;


    /**
    * Root logger for the thread, displays entry/exits of functions
    */
    static Logger* rootLogger_ ;

    /**
    * Indicates wether instruments trace entry/exit call to standard output
    */
    bool trace_ ;


    /**
     * Output filename for logger.
     */
    std::string filename_ ;

    /**
    * Internal tabs counter for logger display.
    */
    int tab_ ;

    /**
     * Output format
     */
    FormatEnum format_ ;

    /**
    * Buffer indicating existence of child for displaying tree.
    */
//    static __thread std::vector<bool>* threadScopeChildExist_ ;

    /**
    * Test if a name must be filtered inside this logger context.
    * @name_ string to be filtered.
    * @return true if name must be filtered, false else.
    */
    bool isFiltered(const std::string& name_)
        __attribute__((no_instrument_function)) ;

    /**
    * Write the scope entry on stdout.
    * @param scope_ Scope to log.
    */
    void logScopeEntry(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Write the scope exit on stdout.
    * @param scope_ Scope to log.
    */
    void logScopeExit(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Write the logger.
    */
    void log()
        __attribute__((no_instrument_function)) ;

    /**
    * Add a column to a logger.
    * @param type_ type of the column to add.
    */
    void addColumn(Column::ColEnum type_)
        __attribute__((no_instrument_function)) ;

private:

    struct LogScopeData
    {
        std::string name ;
        std::string source ;
        void* address ;
        uint64_t id ;
        uint64_t parentId ;
        uint64_t call ;

        uint64_t time_ ;
    } ;

    /**
    * Recursivity depth for tree logging.
    */
    static thread_local int threadTab_ ;


    // this is pretty nasty but avoid infinite loop because called from default constructor
    Logger(bool) ;

    /**
    * Log Columns content to stream.
    * @param logger_ Logger containing data.
    * @param fout_ Stream to log to.
    */
    void logColumns(std::ostream &fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Add the scope data logger columns.
    * @param _data Scope data to add.
    * @param scope_ Scope to output.
    * @param fout_ Stream to output to.
    */
    void logScope(LogScopeData* _data)
        __attribute__((no_instrument_function)) ;




    /**
    * Log scopes to a stream with a text output.
    * @param fout_ Stream to log to.
    */
    void logTextScopes(std::ostream &fout_)
        __attribute__((no_instrument_function)) ;

    /**
    * Log scopes to a stream with a text output recursively.
    * @param fout_ Stream to log to.
    */
    void recursiveLogTextScopes(Scope* _scope)
        __attribute__((no_instrument_function)) ;

    void logJsonScopes(std::ostream &fout) ;

    /**
    * Log scopes to a stream with a json output recursively.
    * @param fout_ Stream to log to.
    */
    void recursiveLogJsonScopes(Scope* _scope, std::ostream &fout)
        __attribute__((no_instrument_function)) ;


    static void printTabs(int tabNum_)
         __attribute__((no_instrument_function)) ;
    static void printEntryExit(bool entry_)
         __attribute__((no_instrument_function)) ;
    static void printTree(Scope* data_)
         __attribute__((no_instrument_function)) ;


} ;

}


#endif
