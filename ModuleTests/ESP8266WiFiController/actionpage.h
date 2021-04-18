#ifndef ACTIONPAGE_H
#define ACTIONPAGE_H

const char actionpage[] PROGMEM = R"=====(
<div class="content">
    <h4 class="center">Movements</h4>
    <div class="btn center" style="width: 100%;">
        <div class="radiolist">
            Walk <input id="walk" name="movementType" type="radio" value="wk" checked>
        </div>
        <div class="radiolist">
            Run <input id="run" name="movementType" type="radio" value="rn">
        </div>
        <div class="radiolist">
            Trot <input id="trot" name="movementType" type="radio" value="tr">
        </div>
        <div class="radiolist">
            Crawl <input id="crawl" name="movementType" type="radio" value="cr">
        </div>
    </div>
    <div class="btn center" style="width: 100%;">
        <button onclick="movement('F')">Forward</button>
    </div>
    <div class="btn" style="width:100%">
        <button onclick="movement('L')">Forward Left</button>
        <button onclick="movement('R')">Forward Right</button>
    </div>
    <div class="btn" style="width:100%">
        <button onclick="location.href='/actionpage?cmd=kbkL'">Back Left</button>
        <button onclick="location.href='/actionpage?cmd=kbkR'">Back Right</button>
    </div>
    <div class="btn center" style="width: 100%;">
        <button onclick="location.href='/actionpage?cmd=kbk'">Back</button>
    </div>
    <h4 class="center">Mode</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=g'">GyroOn/Off</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=c'">Calibration</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kbalance'">Balanced</button>
    </div>
    <h4 class="center">Actions</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=ksit'">Sit</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kck'">Check</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kbuttUp'">Buttup</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=khi'">Hello</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kstr'">Stretch</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kpee'">Pee</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kpu'">Push ups</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kvt'">Stepping</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=lu'">Look up</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kbf'">Backflip</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=kbdF'">Jumps</button>
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=rc'">Recover</button>
    </div>
    <div class="btn-group btn-stop" style="width:100%">
        <button style="width: 100%" onclick="location.href='/actionpage?cmd=d'">STOP</button>
    </div>
    <h4 class="center">Custom command</h4>
    <form action="/action" method="get">
        <div class="center" style="width: 100%; margin-top: 25px;">
            <input name="name" type="text" placeholder="input command"><input type="submit"/>
        </div>
    </form>
    <script type="text/javascript">
        document.addEventListener('DOMContentLoaded', function() {
            let query = window.location.search;
            const urlParams = new URLSearchParams(query);
            if (urlParams.has('cmd')) {
                let cmd = urlParams.get('cmd');
                if (cmd.startsWith('kwk')) {
                    document.getElementById('walk').checked = true;
                } else if (cmd.startsWith('krn')) {
                    document.getElementById('run').checked = true;
                } else if (cmd.startsWith('kcr')) {
                    document.getElementById('crawl').checked = true;
                } else if (cmd.startsWith('ktr')) {
                    document.getElementById('trot').checked = true;
                }
            }
        });

        function movement(direction) {
            const movementType = document.querySelector('input[name="movementType"]:checked').value;
            window.location.href = `/actionpage?cmd=k${movementType}${direction}`;
        }
    </script>
</div>
)=====";

#endif
