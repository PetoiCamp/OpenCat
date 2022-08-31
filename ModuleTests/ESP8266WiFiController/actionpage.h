#ifndef ACTIONPAGE_H
#define ACTIONPAGE_H

const char actionpage[] PROGMEM = R"=====(
<div class="content">
    <h4 class="center">Movements</h4>
    <div class="btn center" style="width: 100%;">
        <button onclick="httpGet('/actionpage?name=forward')">Forward</button>
    </div>
    <div class="btn" style="width:100%">
        <button onclick="httpGet('/actionpage?name=forwardleft')">Forward Left</button>
        <button onclick="httpGet('/actionpage?name=forwardright')">Forward Right</button>
    </div>
    <div class="btn" style="width:100%">
        <button onclick="httpGet('/actionpage?name=backleft')">Back Left</button>
        <button onclick="httpGet('/actionpage?name=backright')">Back Right</button>
    </div>
    <div class="btn center" style="width: 100%;">
        <button onclick="httpGet('/actionpage?name=back')">Back</button>
    </div>
    <h4 class="center">Mode</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=gyro')">GyroOn/Off</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=calibration')">Calibration</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=balance')">Balanced</button>
    </div>
    <h4 class="center">Actions</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=walk')">Walk</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=trot')">Trot</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=crawl')">Crawl</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=sit')">Sit</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=check')">Check</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=buttup')">Buttup</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=hello')">Hello</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=stretch')">Stretch</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=run')">Run</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=pee')">Pee</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=pushup')">Push ups</button>
        <button style="width: 100%" onclick="httpGet('/actionpage?name=stepping')">Stepping</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=lookup')">Look up</button>
    </div>
    <div class="btn-group btn-stop" style="width:100%">
        <button style="width: 100%" onclick="httpGet('/actionpage?name=stop')">STOP</button>
    </div>
    <h4 class="center">Custom command</h4>
    <form action="/actionpage" method="get">
        <div class="center" style="width: 100%; margin-top: 25px;">
            <input name="name" type="text" placeholder="input command"><input type="submit"/>
        </div>
    </form>
</div>
)=====";

#endif
