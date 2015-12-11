'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser');

/**
 * Service used to manage the ftrace json data
 */
ftraceAnalyser.service('jsonloader', [ 
function () {
	this.data = null ;
	
	this.load = function(onOk) {
		var me=this ;
		
		$log.debug("load json") ;
		var oReq = new XMLHttpRequest();  
		oReq.onload = function() { JSON.parse(this.responseText); me.data=eval("("+this.responseText+")"); onOk() ;};  
		oReq.onerror = function() { $log.debug("error loadin json");};  ;  
		oReq.open('get', '/app/stats.json', true);  
		oReq.send();
	};
	
	this.destroy = function () {
	};
		
}])