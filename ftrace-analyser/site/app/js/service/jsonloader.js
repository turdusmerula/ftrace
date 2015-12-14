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
	this.calls = [] ;
	
	this.load = function(onOk) {
		var me=this ;
		
		$log.debug("load json") ;
		var oReq = new XMLHttpRequest();  
		oReq.onload = function() { 
			JSON.parse(this.responseText) ; 
			me.data = eval("("+this.responseText+")") ;
			me.functions = me.data.functions ;
			me.threads = me.data.threads ;
			me.calls = [] ;
			me.computeCalls() ;
			// call callback
			onOk() ;
		};  
		oReq.onerror = function() { $log.debug("error loadin json");};  ;  
		oReq.open('get', '/app/stats.json', true);  
		oReq.send();
	
	};
	
	this.destroy = function () {
	};
	
	this.computeCalls = function () {
		var tl = this.threads.length;
		for (var ith=0 ; ith<tl ; ith++) {
			var thread = this.threads[ith] ;
			thread.calls = [] ;
			
			if(thread.hasOwnProperty('scopes'))
				this.recursiveComputeCalls(thread.scopes, thread) ;
		}
	} ;
	
	this.recursiveComputeCalls = function(scopes, thread) {
		$log.debug("recursiveComputeCalls", scopes, thread) ;
		var sl = scopes.length;
		for (var isc=0 ; isc<sl ; isc++) {
			var scope = scopes[isc] ;
			
			// Add scope timing and calls
			this.addScopeData(scope, thread) ;
			this.addScopeData(scope, this) ;
			
			if(scope.hasOwnProperty('scopes'))
				this.recursiveComputeCalls(scope.scopes, thread) ;
		}	
	} ;
	
	this.addScopeData = function(scope, callsOwner) {
	
		var calls=callsOwner.calls ;
		var call=null ;
		
		// search for call
		var cl = calls.length ;
		for (var icl=0 ; icl<cl ; icl++) {
			if(calls[icl].addr===scope.addr) {				
				call = calls[icl] ;
				break ;
			}
		}
		
		if(call===null){
			call = {"addr": scope.addr, "calls": scope.calls, "time": scope.time} ;
			call.name = this.getFunction(scope.addr).name ;
			calls.push(call) ;
		} else {
			call.calls += scope.calls ;
			call.time += scope.time ;
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