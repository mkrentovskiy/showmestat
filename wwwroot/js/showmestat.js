(function($) {
    var ws = null;
    
    var t = {}; // Templates
    var ti = {}; // Init values
    var statistics = ['flows', 'hosts']; // 'protocols',
    
    var flows_protocols = [];
    var flows_timeline = [];
    var MAX_TIMELINE_ELEMENTS = 60;

    var host_tr_def = "unknown";
    var hosts_tr = [
            ["Android", "android"],
            ["Apple", "apple"],
            ["BSD", "bsd"],
            ["CD-Based OSes", "cd"],
            ["Datacenter appliance", "datacenter"],
            ["Dead OSes", "dead"],
            ["Gaming Consoles", "console"],
            ["Home Audio/Video Equipment", "avhome"],
            ["Linux", "linux"],
            ["Misc", "misc"],
            ["Monitoring Devices", "monitor"],
            ["Network Boot Agents", "net"],
            ["Physical Security", "security"],
            ["Point of Sale devices", "pos"],
            ["Printers", "printer"],
            ["Projectors", "projector"],
            ["Routers and APs", "router"],
            ["Smartphones/PDAs/Tablets", "tablet"],
            ["Solaris", "solaris"],
            ["Storage Devices", "storage"],
            ["Switches", "switch"],
            ["Thin Clients", "thin"],
            ["Video Conferencing", "vc"],
            ["VoIP Phones/Adapters", "voip"],
            ["Windows", "windows"],
            ["Unknown", host_tr_def]
        ];


    $.app = {}
    $.app.init = function() {
        ui_init();

        window.WebSocket = window.WebSocket || window.MozWebSocket;
        connect(true);
    }

    /*
        Communications
    */

    function repeat(f) { setTimeout(f, 1000) };
    function connect(first_time) {
        ui_con_state('connecting');

        ws = new WebSocket('ws://' + window.location.host + '/', 'stat');
        ws.onopen = function() {
            ui_con_state('connected');            
            if(first_time) {
                $.map(statistics, function(v, k) { repeat(function() { get(v); }); });
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
        $.map(statistics, function(v, i) { ti[v] = $("." + v).html(); });
        $.map($("._template"), function(v, i) { t[$(v).attr('id')] = $(v).html(); });
    };

    function ui_con_state(state) { $(".con_state").hide(); $(".con_state_" + state).show(); }
    function ui_apply(cl, data, cb) {
        $("." + cl).html(ti[cl] + 
            $.map(data, function(v, k) { return ui_apply_template(cl, atr(cl, v)); }).join(''));
        if(cl == "flows") ui_graphs(data);
        else if(cl == "protocols") ui_charts(data);
        if(cb) cb();
    }

    function atr(cl, d) {
        if(cl == "hosts") {
            d.push(hosts_tr.reduce(function(prev, curr, i, a) { 
                    return (curr[0] == d[1]) ? curr[1] : prev; 
                }, host_tr_def));
        } else if(cl == "protocols") {
            for(i = 1; i < 5; i++) d.push(d[i].replace(/(\d)(?=(\d\d\d)+([^\d]|$))/g, '$1 '));
        }
        return d;
    }
    function ui_apply_template(cl, data) {        
        return data.reduce(function(prev, curr, i, a) { return prev.replace(new RegExp("@" + i + "@", 'g'), curr); }, t[cl]);
    }

    /*
        graphs
    */
    function ui_graphs(data) {
        var cnts = {};
        $.map(data, function(v, k) { 
                var proto = v[3];
                
                if(cnts[proto]) cnts[proto]++;  
                else {
                    cnts[proto] = 1;
                    if(flows_protocols.indexOf(v[3]) == -1) flows_protocols.push(v[3]);
                }
            });

        var now = [{ key: "By protocols", 
                     values: $.map(cnts, function(v, k) { return {"label": k, "value": v}; }) }];

        cnts["_now"] = Date.now();
        if(flows_timeline.length >= MAX_TIMELINE_ELEMENTS) flows_timeline.shift(); 
        flows_timeline.push(cnts);   

        // current state
        nv.addGraph(function() {
            var chart = nv.models.discreteBarChart()
                            .x(function(d) { return d.label })
                            .y(function(d) { return d.value })
                            .valueFormat(d3.format('d'))
                            .staggerLabels(true)
                            .tooltips(false)
                            .showValues(true)
                            .duration(250);

            d3.select('#bar_flows svg').datum(now).call(chart);
            return chart;
        });

        // timeline
        var tl = $.map(flows_protocols, function(v, k) {
                return { key: v, 
                         values: 
                            $.map(flows_timeline, function(vv, kk) { 
                                    return { x: (vv['_now']), y: (vv[v] ? vv[v] : 0)}; 
                                }) }
            });
        nv.addGraph(function() {
            var chart = nv.models.multiBarChart()
                .width($("#bar_flows_timeline").width())
                .height($("#bar_flows_timeline").height())
                .stacked(false);
 
            chart.xAxis.tickFormat(function(d) { return d3.time.format('%M:%S')(new Date(d)) });
            chart.yAxis.tickFormat(d3.format('d'));
            d3.select('#bar_flows_timeline svg').datum(tl).call(chart);
            return chart;
        });

    }


    /*
        charts
    */

    function ui_charts(d) {
        for(i = 1; i < 5; i++) chart($("#g" + i).html("").toArray(), 
                                     $.map(d, function(v, k) { return v[0]; }),
                                     $.map(d, function(v, k) { return parseInt(v[i]); }),
                                     300,
                                     300);   
    }
    function chart(place, headers, data, w, h) {
        var outerRadius = w / 2;
        var innerRadius = w / 15;

        var color = d3.scale.category20();
        var pie = d3.layout.pie();
        var m = data.reduce(function(pv, cv) { return pv + cv; }, 0);
        var arc = d3.svg.arc().innerRadius(innerRadius).outerRadius(outerRadius);
        var svg = d3.selectAll(place).append("svg").attr("width", w).attr("height", h);

        var arcs = svg.selectAll("g.arc")
                      .data(pie(data))
                      .enter()
                      .append("g")
                      .attr("class", "arc")
                      .attr("transform", "translate(" + outerRadius + ", " + outerRadius + ")");

        arcs.append("path")
            .attr("fill", function(d, c) { return color(c);})
            .attr("d", arc);
        
        arcs.append("text")
            .attr("transform", function(d) { return "translate(" + arc.centroid(d) + ")"; })
            .attr("text-anchor", "middle")
            .text(function(d) {
                    return ((d.value * 100 / m) > 10) ? headers[(data.indexOf(d.value))] : "";
                });
    }


})(window.jQuery);

$(document).ready(function() { $.app.init(); });