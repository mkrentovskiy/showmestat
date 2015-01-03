(function($) {
    var ws = null;
    var repeat = function(f) { setTimeout(f, 1000) };
    
    $.app = {}
    $.app.init = function() {
        window.WebSocket = window.WebSocket || window.MozWebSocket;
        connect(true);
    }

    function connect(first_time) {
        ui_con_state('connecting');

        ws = new WebSocket('ws://' + window.location.host + '/', 'stat');
        ws.onopen = function() {
            ui_con_state('connected');            
            if(first_time) {
                repeat(function() { get('flows'); });
                repeat(function() { get('hosts'); });
            }
        }
        ws.onerror = function() {
            ui_con_state('error');
            repeat(function() { connect(false); });
        }
        ws.onclose = function() {
            ui_con_state('connecting');
            window.location.reload();
        }
        ws.onmessage = function(m) { 
            if(m.data) { 
                obj = JSON && JSON.parse(m.data) || $.parseJSON(m.data);
                console.log(obj);
            } 
        }
    }

    function get(t) {
        if(ws.readyState == ws.OPEN) {
            ws.send(t);
        } 
        repeat(function(){ get(t); });
    }

    /*
        UI
    */
    function ui_con_state(state) { $(".con_state").hide(); $(".con_state_" + state).show(); }

})(window.jQuery);

$(document).ready(function() { $.app.init(); });
