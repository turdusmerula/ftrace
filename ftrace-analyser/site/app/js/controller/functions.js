'use strict';

var ftraceAnalyser = angular.module('ftraceAnalyser');

ftraceAnalyser.controller('FunctionsCtrl', [ '$scope', 'jsonloader',
function($scope, jsonloader) {
	$scope.functions = jsonloader.functions ;
	$scope.searchText = '' ;
	$scope.state = {
        sortKey: 'addr',
        sortDirection: 'DEC'
      }

	$scope.functionsTableColumnDefinition = [
	    {
	    	columnHeaderDisplayName: 'Address',
	    	displayProperty: 'addr',
	    	sortKey: 'addr',
	    	columnSearchProperty: 'addr',
	    	visible: true,
	    	width: '8em'
	    },
	    {
	    	columnHeaderDisplayName: 'Source',
	    	displayProperty: 'source',
	    	sortKey: 'source',
	    	columnSearchProperty: 'source',
	    	visible: true,
	    	width: '20em'
	    },
	    {
	    	columnHeaderDisplayName: 'Name',
	    	displayProperty: 'name',
	    	sortKey: 'name',
	    	columnSearchProperty: 'name',
	    	visible: true
	    }
    ] ;
	
	$scope.$on('json-loaded', function() {
		$log.debug("refresh functions") ;
		$scope.functions = jsonloader.functions ;
        $scope.$apply();
	}) ;
	
}]);
