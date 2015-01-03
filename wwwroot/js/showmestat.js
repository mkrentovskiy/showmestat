(function($) {
    var ws = null;

    $.app = {}
    $.app.init = function() {
        window.WebSocket = window.WebSocket || window.MozWebSocket;
        connect();
    }

    function connect() {
        ui_con_state('connecting');

        ws = new WebSocket('ws://' + window.location.host + '/', 'stat');
        ws.onopen = function() {
            ui_con_state('connected');            
        }
        ws.onerror = function() {
            ui_con_state('error');
            setTimeout(connect, 1000);
        }
        ws.onmessage = function(m) { if(m.data) process(m.data); }
    }


    function process(d) {
        console.log(d);
    }

    /*
        UI
    */
    function ui_con_state(state) { $(".con_state").hide(); $(".con_state_" + state).show(); }

})(window.jQuery);

$(document).ready(function() { $.app.init(); });
