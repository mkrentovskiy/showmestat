(function($) {
    
    $.app = {}
    $.app.init = function() {
        ui_con_state("connecting");
    }

    /*
        UI
    */
    function ui_con_state(state) { $(".con_state").text(state); }

})(window.jQuery);

$(document).ready(function() { $.app.init(); });
