const char htmltxt[] PROGMEM = R"(
<html>

<head>
    <title>四足机器人控制</title>
    <meta charset="UTF-8">
    <meta name='viewport' content='width=device-width,initial-scale=1' />
    <style>
        .btn {
            margin-left: 10px;
            text-align: center;
            width: 60px;
            height: 30px;
        }
        
        .gengai {
            width: 290px;
            height: 100px;
            resize: none;
            margin-top: 10px;
            margin-right: 0px;
            margin-bottom: 0px;
            margin-left: 10px;
        }
        
        .file {
            text-align: center;
            width: 220px;
            height: 30px;
            margin-left: 10px;
        }
        
        b {
            color: red;
            font-size: 18px;
        }
    </style>

</head>

<body>
    <div align="center">

        <p id="content">
            <font color="#000">返回:</font>
        </p>
        <b>发送(0-xx)</b>
        <input class="btn" name="cmdstr" type="text" id="textId" />
        <button class="btn" type="button" onClick="sendpost();">发送</button></br>
        </br>
        <!--<button class="btn" type="button" onClick=" sendcmd(1);">待机</button>-->
        <button class="btn" type="button" onClick="sendcmd(2);">前进</button>
        <button class="btn" type="button" onClick="sendcmd(3);">后退</button>
        <button class="btn" type="button" onClick="sendcmd(4);">左移</button>
        <button class="btn" type="button" onClick="sendcmd(5);">右移</button>
        </br>
        </br>
        <button class="btn" type="button" onClick="sendcmd(6);">左转</button>
        <button class="btn" type="button" onClick="sendcmd(7);">右转</button>
        <button class="btn" type="button" onClick="sendcmd(8);">趴下</button>
        <button class="btn" type="button" onClick="sendcmd(9);">打招呼</button>
        </br>
        </br>
        <button class="btn" type="button" onClick="sendcmd(10);">战斗</button>
        <button class="btn" type="button" onClick="sendcmd(11);">睡眠</button>
        <button class="btn" type="button" onClick="sendcmd(12);">舞动1</button>
        <button class="btn" type="button" onClick="sendcmd(13);">舞动2</button>
        </br>
        </br>
        <button class="btn" type="button" onClick="sendcmd(14);">舞动3</button>
        <button class="btn" type="button" onClick="sendcmd(15);">俯卧撑</button>
        <button class="btn" type="button" onClick="sendcmd(16);">操作16</button>
        <button class="btn" type="button" onClick="sendcmd(16);">操作17</button>
        </br>
        </br>
        <!---
 </br>
<textarea  class="gi"  name="anlge" type="text" id="gi" />调整角度</textarea>
-->
        </br>
        <!--
//<label><input name="sfile" type="radio" value="Firmware" checked />固件</label> 
//<label><input name="sfile" type="radio" value="Filesystem" />文件</label> 
</br>
//<form method="POST" action="" enctype="multipart/form-data" id="upload_form" >
// <input class="file" type="file"  accept='.bin,.bin.gz,' name="update" id="file">
//  <input  type="submit" value="升级固件">
// </form>
-->
    </div>
    <div id="prg_wrap" style="border: 0px solid; width: 100%;">
        <div id="prg" style="text-shadow: 2px 2px 3px black; padding: 5px 0; display: none; border: 1px solid #008aff; background: #002180; text-align: center; color: white;"></div>
</body>
<script type="text/javascript">
    var sleep = function(time) {
        var startTime = new Date().getTime() + parseInt(time, 10);
        while (new Date().getTime() < startTime) {}
    };

    function getradio() {
        var radio_test = document.getElementsByName('sfile');
        for (var i = 0; i < radio_test.length; i++) {
            if (radio_test[i].checked) {
                return radio_test[i].value;
            }
        }
        return 0;
    }

    function sendget(cmd) {
        //var url = 'http://192.168.0.89';
        var url = window.location.href;
        const urlToExpand = url + '?abb=' + cmd;
        //window.alert(urlToExpand);
        const xhr = new XMLHttpRequest;
        xhr.responseType = 'html';
        xhr.onreadystatechange = function() {
            if (xhr.readyState == XMLHttpRequest.DONE) {
                var status = "返回:";
                var fullname = status + xhr.responseText;
                document.getElementById("content").innerText = fullname;
            }
        }
        xhr.open('GET', urlToExpand);
        xhr.send();
    }


    function sendpost() {
        var cmd = document.getElementById("textId").value;
        sendcmd(cmd);
    }

    function sendcmd(cmd) {
        var cmdtxt = "cmdstr=" + cmd;
        //window.alert(cmdtxt);
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "", true);
        xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                var status = "返回:";
                var fullname = status + xhr.responseText;
                document.getElementById("content").innerText = fullname;

            }
        }
        xhr.send(cmdtxt); // http.send("username=xx&password=yy");
    }


    var domReady = function(callback) {
        document.readyState === "interactive" || document.readyState === "complete" ? callback() : document.addEventListener("DOMContentLoaded", callback);
    };

    domReady(function() {
        var myform = document.getElementById('upload_form');
        var filez = document.getElementById('file');


        myform.onsubmit = function(event) {
            event.preventDefault();

            var formData = new FormData();
            var file = filez.files[0];
            if (!file) {
                return false;
            }

            formData.append("files", file, getradio());
            //formData.append("files", file, file.name);

            var xhr = new XMLHttpRequest();
            xhr.upload.addEventListener("progress", function(evt) {
                if (evt.lengthComputable) {
                    var per = Math.round((evt.loaded / evt.total) * 100);
                    var prg = document.getElementById('prg');

                    prg.innerHTML = per + "%"
                    prg.style.width = per + "%"
                    prg.style.display = "block"
                }
            }, false);
            xhr.open('POST', location.href, true);

            // Set up a handler for when the request finishes.
            xhr.onload = function() {
                if (xhr.status === 200) {
                    var status = "返回:";
                    var fullname = status + this.responseText;
                    document.getElementById("content").innerText = fullname;
                    //alert('Success');
                } else {
                    //alert('An error occurred!');
                }
            };

            xhr.send(formData);
        }
    });
</script>

</html>


)";