'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser');

/**
 * Service used to manage the ftrace json data
 */
ftraceAnalyser.service('jsonloader', [ 
function () {
	this.data = {} ;
	
	this.functions = [] ;
	this.threads = [] ;
	this.func_calls = [] ;
	
	this.load = function(onOk) {
		var me=this ;
		
		$log.debug("load json") ;
		var oReq = new XMLHttpRequest();  
		oReq.onload = function() { 
			JSON.parse(this.responseText) ; 
			me.data = eval("("+this.responseText+")") ;
			me.functions = me.data.functions ;
			me.threads = me.data.threads ;
			me.func_calls = [] ;
			me.compute() ;
			// call callback
			onOk() ;
		};  
		oReq.onerror = function() { $log.debug("error loadin json");};  ;  
		oReq.open('get', '/app/stats.json', true);  
		oReq.send();
	
	};
	
	this.destroy = function () {
	};
	
	/**
	 * Summarise calls and timings for function in each thread and process
	 */
	this.compute = function () {
		var tl = this.threads.length;
		for (var ith=0 ; ith<tl ; ith++) {
			var thread = this.threads[ith] ;
			thread.func_calls = [] ;
			
			if(thread.hasOwnProperty('scopes')) {
				this.recursiveComputeReal(thread, thread) ;
				this.recursiveComputeInner(thread, thread) ;
			}
		}
	} ;
	
	this.recursiveComputeReal = function(scope, thread) {
		var sl = scope.scopes.length ;
		for (var isc=0 ; isc<sl ; isc++) {
			var child_scope = scope.scopes[isc] ;
			
			// Add scope timing and calls
			this.addScopeData(child_scope, thread) ;
			this.addScopeData(child_scope, this) ;
			
			// set scope parent
			child_scope.parent = scope ;
			
			// compute real time
			child_scope.real = child_scope.time-child_scope.inst ;
			
			// alter all parents chain
			var parent = child_scope.parent ;
			while(parent!=null)	{
				parent.real -= child_scope.inst ;
				parent = parent.parent ;
			}

			if(child_scope.hasOwnProperty('scopes'))
				this.recursiveComputeReal(child_scope, thread) ;
		}	
	} ;
	
	this.recursiveComputeInner = function(scope, thread) {
		var sl = scope.scopes.length ;
		for (var isc=0 ; isc<sl ; isc++) {
			var child_scope = scope.scopes[isc] ;
			
			// Add scope timing and calls
			this.addScopeData(child_scope, thread) ;
			this.addScopeData(child_scope, this) ;
			
			// set scope parent
			child_scope.parent = scope ;
			
			// compute inner timer
			child_scope.inner = child_scope.real ;
			child_scope.parent.inner -= child_scope.real ;
			
			if(child_scope.hasOwnProperty('scopes'))
				this.recursiveComputeInner(child_scope, thread) ;
		}	
	} ;

	/**
	 * Add a scope data to global scope summary, if it does not exists create it
	 */
	this.addScopeData = function(scope, callsOwner) {
	
		var func_calls=callsOwner.func_calls ;
		var func_call=null ;
		
		// search for call
		var cl = func_calls.length ;
		for (var icl=0 ; icl<cl ; icl++) {
			if(func_calls[icl].addr===scope.addr) {				
				func_call = func_calls[icl] ;
				break ;
			}
		}
		
		if(func_call===null){
			func_call = {"addr": scope.addr, "calls": scope.calls, "time": scope.time, "real": scope.time-scope.inst} ;
			func_call.name = this.getFunction(scope.addr).name ;
			func_calls.push(func_call) ;
		} else {
			func_call.calls += scope.calls ;
			func_call.time += scope.time ;
			func_call.real = scope.time-scope.inst ;
		}
	} ;
	
	this.getFunction = function(addr) {
		var func = null ;
		
		// search for call
		var cf = this.functions.length ;
		for (var ift=0 ; ift<cf ; ift++) {
			if(this.functions[ift].addr===addr) {				
				func = this.functions[ift] ;
				break ;
			}
		}
		
		return func ;
	} ;
}])