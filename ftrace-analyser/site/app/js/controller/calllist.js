'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser');

ftraceAnalyser.controller('CallListCtrl', [ '$scope', 'jsonloader',
function($scope, jsonloader) {
	$scope.functions = jsonloader.functions ;
	$scope.threads = jsonloader.threads ;
	$scope.thread = {} ;
	$scope.calls = {} ;
	$scope.allThreads = jsonloader ;
	
	$scope.searchText = '' ;
	$scope.state = {
        sortKey: 'addr',
        sortDirection: 'DEC'
      }

	$scope.calllistTableColumnDefinition = [
	    {
	    	columnHeaderDisplayName: 'Address',
	    	displayProperty: 'addr',
	    	sortKey: 'addr',
	    	columnSearchProperty: 'addr',
	    	visible: true,
	    	width: '8em'
	    },
	    {
	    	columnHeaderDisplayName: 'Name',
	    	displayProperty: 'name',
	    	sortKey: 'name',
	    	columnSearchProperty: 'name',
	    	visible: true,
	    	width: '40em'
	    },
	    {
	    	columnHeaderDisplayName: 'Calls',
	    	displayProperty: 'calls',
	    	sortKey: 'calls',
	    	columnSearchProperty: 'calls',
	    	visible: true
	    },
	    {
	    	columnHeaderDisplayName: 'Time',
	    	displayProperty: 'time',
	    	sortKey: 'time',
	    	columnSearchProperty: 'time',
	    	visible: true
	    }
    ] ;
	
	$scope.$on('json-loaded', function() {
		$log.debug("refresh functions") ;
		$scope.functions = jsonloader.functions ;
		$scope.threads = jsonloader.threads ;
		$scope.calls = jsonloader.func_calls ;
		$scope.thread = this ;
        $scope.$apply();
	}) ;

	
    $scope.select = function(item) {
    	$scope.thread = item ;
    	$scope.calls = item.func_calls ;
    	$log.log('Selected: ', item);
    };

		
}]);
