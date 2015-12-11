'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser');

ftraceAnalyser.controller('TreeViewCtrl', [ '$scope', 'jsonloader',
	function($scope, jsonloader) {

		$scope.openjson = function() {
			jsonloader.load(
				function() {
					$log.debug("loaded") ;
					$scope.result = JSON.stringify(jsonloader.data) ;
				}
			) ;
		};
		
	} 
]);
