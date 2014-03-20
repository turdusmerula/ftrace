ftrace
======

Thread safe gcc function entry/exit tracer 

Getting started
=====

ftrace is a small helper designed for gcc users on Linux x86 platforms.
This tool has 3 main purposes:
  * tracing entry/exit of functions during runtime,
  * reverse engineering by providing per thread call graph output,
  * helping in optimizations by detecting critical code. 

Using a debugger is useful in many cases but can be very painful when using pieces of software written by another team. Often to understand code one adds a lot of trace everywhere, although being very time consuming is not very helpful with very huge code. This tool was mainly created to speed up this task by having a good overview of internal behavior with just few modifications on makefiles. 
Impact on performances is not null but remains acceptable in most cases.

How does it work
=====

gcc offers the capability to instrument functions, when the compile option "`instrument-functions`" is given gcc automatically adds a call upon entry and exit of every instrumented function:

    void __cyg_profile_func_enter(void *this_fn, void *call_site) ;
    void __cyg_profile_func_exit(void *this_fn, void *call_site) ;

The entry and exit functions are not mandatory, if they are not declared they are simply not called. This comportment is used by the logger, the functions are contained in a dynamic library and are added only on demand at runtime.

Configuration
=====

When requested ftrace logs on standard output every entry/exit trace for each called function. 
A statistic file is produced on `/tmp` containing call graphs with requested filters.

Output is controled through a configuration file with template:

    #Flag activating/disactivating logger
    trace=true
 
    #Patterns to ignore in logger, patterns are treaded by apparition order, if a pattern is matched more than once then the last match is taken.
    #ignore .*
    #include b<double>
    #include g
    #include b.*
    #include c
 
    #Adds a logger in output file:
    #Syntax: logger=<type>/<unit>/<columns>
    #<type>:
    # global: generate a list with all calls for each function
    # tree: generate a coverage tree
    #<unit>:
    # auto: use the appropriate unit to display time
    # second: display in seconds
    # ticks: display processor ticks (usefull in case of variable frequency processor)
    #<colmuns>:
    # %a : Number of calls
    # %b : Names of the called functions
    # %c : Names of the called functions in tree format (same as %b for a global logger)
    # %d : Total time elapsed in calls
    # %e : Total time divided by number of calls
    # %f : Total time elapsed in scope without time in sub-scopes
    # %g : Total time elapsed in scope without time in sub-scopes divided by number of calls
    # %h : Total time elapsed in instrument code
    # %i : Total time elapsed in instrument code divided by number of calls
    # %j : Undocumented
    # %k : Undocumented
    # %l : Real time elapsed in scope without time elapsed in instrument code
    # %m : Real time elapsed in scope without time elapsed in instrument code divided by number of call
    # %n : Adress of function

    logger=tree/ticks/%a%c%d%e%n%o%p%q%r%s%t
 
    logger=tree/second/%a%c%d%e%n
    ignore .*
    include main.*
    include e
    include f
    include g
    include std::list.*
 
    #logger=tree/second/%a%c%d%e
    #ignore std::list.*
 
    logger=global/ticks/%a%b%d%e%n
    ignore .*
    include main.*

Output
=====

The statistic file generated on /tmp will contain an output of this form:

    _______________________________________________________________________________
 
    Calls  |Name                                                          |Total T      |Call T       |Address   |
    1      |+ std::list<test*, std::allocator<test*> >::list              |0.000376454  |0.000376454  |0x402fd2  |
    1      |+ main+0x19                                                   |0.724874     |0.724874     |0x40267a  |
    1      |: + std::list<int, std::allocator<int> >::list                |0.00035573   |0.00035573   |0x403006  |
    2      |: + std::list<int, std::allocator<int> >::push_back           |0.122974     |0.061487     |0x403706  |
    2      |: : + std::list<int, std::allocator<int> >::end               |8.86573e-05  |4.43284e-05  |0x402cae  |
    2      |: : + std::list<int, std::allocator<int> >::_M_insert         |0.00105736   |0.00052868   |0x403682  |
    2      |: :   + std::list<int, std::allocator<int> >::_M_create_node  |0.00096277   |0.000481385  |0x4035de  |
    8      |: + e                                                         |0.25646      |0.0320576    |0x402498  |
    80     |: : + f                                                       |0.198054     |0.00247567   |0x40238c  |
    80     |: :   + g                                                     |1.37272e-05  |1.71188e-07  |0x402b2b  |
    1      |: + std::list<int, std::allocator<int> >::~list               |0.204071     |0.204071     |0x40330e  |
    1      |+ std::list<test*, std::allocator<test*> >::~list             |0.000393857  |0.000393857  |0x40318a  |

Building ftrace
=====

To build ftrace you will need boost and binutils. Boost may be easily available for your linux release but you will need a tweaked version of binutils (mainly for libiberty), this library isn't built against the -fPIC on every system. 

for Debian systems binutils comes with a PIC version of libiberty, just install binutils-dev package
    
    apt-get install binutils-dev

For other systems that may not have a prebuilt PIC version of the lib:
  * First download binutils at [http://ftp.gnu.org/gnu/binutils/].
  * Untar the archive file somewhere (using tar -xf <file>), then move to the freshen created directory.
  * Then compile binutils:

    `CFLAGS=-fPIC ./configure`
    `make`
    
  * You will need the two following generated library to link ftrace:

    `binutils/libiberty/libiberty.a`
    `binutils/bfd/bfd.a`

Now download last version of ftrace from [https://github.com/turdusmerula/ftrace], or clone from the repository:

    git clone https://github.com/turdusmerula/ftrace

Now generate the makefile, for this you will need to have your boost and binutils correctly installed in system folders:
  
    ./build.sh

Alternatively you can configure the project by hand if you do not have system pathes for libs:

    cmake -DBOOST_INCLUDE_DIR=../boost -DBOOST_REGEX_LIB_DIR=../boost/stage/lib  -DBINUTILS_INCLUDE_DIR=../binutils/include -DLIBIBERTY_LIB_DIR=../binutils/libiberty -DLIBBFD_LIB_DIR=../binutils/bfd .
    make

Installing ftrace
===

For debian only you can install lib on system by using:

    ./install.sh
    
for other systems just copy `libftrace.so` inside system libdirectory or simply call "`make install`", the library libftrace.so will be added on `/lib`.

Using ftrace
===

First of all the code to instrument must be compiled with the "`-finstrument-functions`" option and linked with the "`-rdynamic`" option. Most of makefiles offers a quick way to do this by passing options such as `CFLAGS`, `CPPFLAGS` and `LDFLAGS` to the makefile. 

Ftrace provides a dynamic library `libftrace.so` containing the the instrument. The generated executable is instrumented using the preloading dynamic libraries capabilities of linux.

Export the `LD_PRELOAD` environment variable to `libftrace.so` and the `FTRACE` to the configuration file path:

    export LD_PRELOAD=libftrace.so
    export GSCOPELOG=ftrace.fcf
