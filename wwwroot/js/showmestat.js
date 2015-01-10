(function($) {
    var ws = null;
    var repeat = function(f) { setTimeout(f, 1000) };
    var t = {};

    $.app = {}
    $.app.init = function() {
        ui_init();

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
                repeat(function() { get('protocols'); });
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
                var obj = JSON && JSON.parse(m.data) || $.parseJSON(m.data);
                var cb = (obj.req && obj.req.length) ? function() { repeat(function(){ get(obj.req); }); } : null;
                if(obj.reply && obj.reply.length) 
                    ui_apply(obj.req, proto_parse(obj.reply), cb);
            }             
        }
    }
    
    function get(t) { if(ws.readyState == ws.OPEN) ws.send(t); }       
    function proto_parse(str) { return $.map(str.split(';'), function(v, n) { return Array(v.split('|')); }); }

    /*
        UI
    */

    function ui_init() {
        $.map($("._template"), function(v, i) { t[$(v).attr('id')] = $(v).html(); });
    };

    function ui_con_state(state) { $(".con_state").hide(); $(".con_state_" + state).show(); }
    function ui_apply(cl, data, cb) {
        $("." + cl).html($.map(data, function(v, k) { return ui_apply_template(cl, v); }).join(''));
        // if(cb) cb();
    }

    function ui_apply_template(cl, data) {        
        return data.reduce(function(prev, curr, i, a) { return prev.replace(new RegExp("@" + i + "@", 'g'), curr); }, t[cl]);
    }


})(window.jQuery);

$(document).ready(function() { $.app.init(); });
