'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser', ['ngRoute', 'ngStorage', 'ui.router', 'ui.bootstrap.treeview']) ;

ftraceAnalyser.config([ '$stateProvider', function($stateProvider) {
	$stateProvider
		.state('treeview', {
			url: '/treeview',
			templateUrl: 'views/treeview.html'
		})
		;
	
} ]);

// These services are defined here to allow their usage globally
// By default services needs to be injected every time we need them but some services may be used everywhere.
// These services are then accessed through the $rootScope or directly with their name
var $log;
var $q;
var $rootScope;
var $localStorage;
ftraceAnalyser.run(['$rootScope', '$log', '$q', '$state', '$localStorage',
    function(rootScopeService, logService, qService, $state, localStorage){
		$rootScope = rootScopeService ;
		rootScopeService.$rootScope = $rootScope;

		$rootScope.$storage = localStorage;
		$localStorage = localStorage;
		
		$log = logService ;
		rootScopeService.$log = $log;
	    
	    $q = qService ;
	    rootScopeService.$q = $q;
	    
	    $state.transitionTo('treeview');
} ]);
