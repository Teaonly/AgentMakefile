var agent_name = "MakeAgent";
var agent_talk = 0;
var agent_state = "";

function convertHTML(str) {
    const symbols = {
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        "\"": "&quot;",
        "'": "&apos;",
        "\n": "<br/>"
    }
    for (const symbol in symbols) {
        if (str.indexOf(symbol) >= 0) {
        const newStr = str.replaceAll(symbol, symbols[symbol])
        return newStr
        }
    }
    return str;
}

function date_str() {
    var d = new Date();
    var strDate = d.getFullYear() + "/" + (d.getMonth()+1) + "/" + d.getDate();
    var time = d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds();
    return strDate + " | " + time;

}
function append_user(msg) {
    var htxt = '<div id="your-chat" class="your-chat">';
    htxt = htxt + '<p class="your-chat-balloon">' + msg + '</p>';
    htxt = htxt + '<p class="chat-datetime"><span class="glyphicon glyphicon-ok"></span>' + date_str() + '</p>';
    htxt = htxt + "</div>";

    $("#chat-area").append(htxt);
    var objDiv = document.getElementById("chat-area");
    objDiv.scrollTop = objDiv.scrollHeight;
}
function append_agent(txt) {
    var htxt = '<div id="friends-chat" class="friends-chat">';

    htxt += '<div class="profile friends-chat-photo">';
    htxt += '   <img src="images/agent.png" alt="">';
    htxt += '</div>';

    htxt += '<div class="friends-chat-content">';
    htxt += '   <p class="friends-chat-name">' + agent_name + '</p>';
    htxt += '   <p class="friends-chat-balloon">' + convertHTML(msg) + '</p>';
    htxt += '   <h5 class="chat-datetime">' + date_str() + '</h5>';
    htxt += '</div>';

    htxt += '</div>';

    $("#chat-area").append(htxt);

    var objDiv = document.getElementById("chat-area");
    objDiv.scrollTop = objDiv.scrollHeight;
}

function append_agent(msg) {
    var htxt = '<div id="friends-chat" class="friends-chat">';

    htxt += '<div class="profile friends-chat-photo">';
    htxt += '   <img src="images/agent.png" alt="">';
    htxt += '</div>';

    htxt += '<div class="friends-chat-content">';
    htxt += '   <p class="friends-chat-name">' + agent_name + '</p>';
    htxt += '   <p class="friends-chat-balloon">' + convertHTML(msg) + '</p>';
    htxt += '   <h5 class="chat-datetime">' + date_str() + '</h5>';
    htxt += '</div>';

    htxt += '</div>';

    $("#chat-area").append(htxt);

    var objDiv = document.getElementById("chat-area");
    objDiv.scrollTop = objDiv.scrollHeight;
}

function listen() {
    var jqxhr = $.post("/listen", "", function(msg) {
        var talk_num = msg.talk.length;
        for(var i = agent_talk; i < talk_num; i++) {
            append_agent(msg.talk[i]);
        }
        agent_talk = talk_num;

        // show state
        agent_state = msg.state;
        if ( agent_state == "waitting" ) {
            $("#chat_txt").attr("placeholder", "您现在可以和智能体进行交谈了...");
        } 
        if ( agent_state == "running" ) {
            $("#chat_txt").attr("placeholder", "请耐心等待智能体的回复...");
        }
    })
    .done(function() {
        //alert( "second success" );
        setTimeout(listen, 500);
    })
    .fail(function() {
        alert( "服务器错误" );
    }).always(function() {
        //alert( "finished" );
    });
}

function init() {
    $( "#chat_btn" ).on( "click", function() {
        if ( agent_state == "waitting") {
            var txt = $("#chat_txt").val();
            append_user(txt);
            var jqxhr = $.post("/talk", txt, function(_msg_) {                
            })
            .done(function() {
                //alert( "second success" );
            })
            .fail(function() {
                alert( "服务器错误" );
            }).always(function() {
                //alert( "finished" );
            });
        } else {

        }
    } );

    setTimeout(listen, 500);
}

$( document ).ready(function() {
    init();
});

