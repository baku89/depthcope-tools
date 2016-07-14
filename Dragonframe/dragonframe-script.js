#!/usr/local/bin/node

var fs 		= require('fs');
var argv 	= require('argv');
var osc 	= require('node-osc');

var content = '[' + getDateTime() + ']';

var params = argv.run().targets;

// logging
params.forEach(function(param) {
	content += '\t' + param;
})


// parse
var command = params[3];
var frame = parseInt(params[4]);


var client = new osc.Client('127.0.0.1', 1234);
client.send('/dragonframe/' + command.toLowerCase(), frame, function() {
	client.kill();
});

fs.appendFile(__dirname + '/log.txt', content + '\n');

function getDateTime() {

  var date = new Date();

  var hour = date.getHours();
  hour = (hour < 10 ? "0" : "") + hour;

  var min  = date.getMinutes();
  min = (min < 10 ? "0" : "") + min;

  var sec  = date.getSeconds();
  sec = (sec < 10 ? "0" : "") + sec;

  var year = date.getFullYear();

  var month = date.getMonth() + 1;
  month = (month < 10 ? "0" : "") + month;

  var day  = date.getDate();
  day = (day < 10 ? "0" : "") + day;

  return year + "/" + month + "/" + day + " " + hour + ":" + min + ":" + sec;

}