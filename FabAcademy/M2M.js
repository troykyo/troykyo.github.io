$(document).ready(function(){

    var received = $('#received');


    var socket = new WebSocket("ws://localhost:8080/ws");
     
    socket.onopen = function(){  
      console.log("connected"); 
    }; 

    socket.onmessage = function (message) {
      console.log("receiving: " + message.data);
      received.append(message.data);
      received.append($('<br/>'));
    };

    socket.onclose = function(){
      console.log("disconnected"); 
    };

    var sendMessage = function(message) {
      console.log("sending:" + message.data);
      socket.send(message.data);
    };


    // GUI Stuff


    // send a command to the serial port
    $("#cmd_send").click(function(ev){
      ev.preventDefault();
      var cmd = $('#cmd_value').val();
      sendMessage({ 'data' : cmd});
      $('#cmd_value').val("");
    });
  
    $("#cmd_XUP").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'xu'});
    });
    
    $("#cmd_XDOWN").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'xd'});
    });
    $("#cmd_YL").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'yl'});
    });
    
    $("#cmd_YR").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'yr'});
    });
    $("#cmd_TL").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'tl'});
    });
    
    $("#cmd_TR").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'tr'});
    });
    $("#cmd_RF").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'rf'});
    });
    
    $("#cmd_RF").click(function(ev){
      ev.preventDefault();
      sendMessage({ 'data' : 'rb'});
    });    
    
    $('#clear').click(function(){
      received.empty();
    });


});