'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser');

ftraceAnalyser.controller('CallTreeCtrl', [ '$scope', 'jsonloader',
function($scope, jsonloader) {
	$scope.functions = jsonloader.functions ;
	$scope.threads = jsonloader.threads ;
	$scope.data = {} ;
	$scope.data.root = {"scopes": []} ; 
	$scope.allThreads = jsonloader ;
	
	$scope.getThread = function() {
		return $scope.thread ;
	};
	
	$scope.$on('json-loaded', function() {
		$log.debug("refresh functions") ;
		$scope.functions = jsonloader.functions ;
		$scope.threads = jsonloader.threads ;

    	$scope.data.root.scopes.length = 0 ;
    	$scope.data.root.scopes.push(jsonloader.threads[0]) ;

		$scope.calls = jsonloader.calls ;
        $scope.$apply();
	}) ;

	
    $scope.select = function(item) {
//    	$scope.data.root = item ;
//        $scope.$apply();
    	$scope.data.root.scopes.length = 0 ;
    	$scope.data.root.scopes.push(item) ;
    	$log.log('Selected: ', item);
    };
		
}]);
