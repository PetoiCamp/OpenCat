#ifndef ACTIONPAGE_H
#define ACTIONPAGE_H

// action page html file in char array. 
// PROGMEM : This data should be stored in Flash memory.
const char actionpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>OpenCAT Nybble Web Control</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=0.5, maximum-scale=0.8, user-scalable=yes"/>
    <link rel="stylesheet" href="https://cdn.staticfile.org/twitter-bootstrap/4.3.1/css/bootstrap.min.css">
    <script src="https://cdn.staticfile.org/jquery/3.2.1/jquery.min.js"></script>
    <script src="https://cdn.staticfile.org/popper.js/1.15.0/umd/popper.min.js"></script>
    <script src="https://cdn.staticfile.org/twitter-bootstrap/4.3.1/js/bootstrap.min.js"></script>

    <style>
        .header {
            background-color: #F1F1F1;
            text-align: center;
        }

        h1 {
            padding: 10px;
        }

        ul {
            list-style-type: none;
            margin: 0;
            padding: 0;
            overflow: hidden;
            border: 1px solid #E7E7E7;
            background-color: #F3F3F3;
        }

        li {
            float: left;
            margin-left: 10px;
            margin-right: 10px;
        }

        li a {
            display: block;
            color: #666666;
            text-align: center;
            padding: 14px, 16px;
            text-decoration: none;
        }

        li a:hover:not(.active) {
            background-color: #dddddd;
        }

        li a.active {
            color: #FFFFFF;
            background-color: #4caf50;
        }

        .btn-group button {
            border-radius: 25px;
            background-color: #4CAF50; /* Green background */
            border: 1px solid green; /* Green border */
            color: white; /* White text */
            padding: 10px 24px; /* Some padding */
            cursor: pointer; /* Pointer/hand icon */
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 20px;
            height: 50px;
            margin: 5px;
        }

        .btn-group button:hover {
            background-color: #3e8e41;
        }

        .btn button {
            border-radius: 25px;
            background-color: #2196F3;
            border: 1px #2196F3;
            color: white;
            text-align: center;
            font-size: 25px;
            display: inline-block;
            width: 45%;
            height: 60px;
        }

        .btn-stop button {
            border-radius: 25px;
            background-color: red;
            border: 1px red;
            color: white;
            text-align: center;
            font-size: 25px;
            display: inline-block;
            width: 45%;
            height: 60px;
        }

        .footer {
            width: 100%;
            background-color: #F1F1F1;
            text-align: center;
            position: fixed;
            bottom: 0;
            height: 45px;
            font-size: 15px;
            padding: 10px;
        }

        .center {
            text-align: center;
        }

    </style>
</head>

<body>
<div class="header">
    <ul>
        <li><a href="/">Home</a></li>
        <li><a class="active" href="/actionpage">Actions</a></li>
        <li><a href="/calibrationpage">Calibration</a></li>
    </ul>
    <h1>Action Control Page</h1>
</div>

<div class="content">
    <div class="btn center" style="width: 100%;">
        <button onclick="location.href='/action?name=forward'">Forward</button>
    </div>
    <div class="btn" style="width:100%">
        <button onclick="location.href='/action?name=forwardleft'">Forward Left</button>
        <button onclick="location.href='/action?name=forwardright'">Forward Right</button>
    </div>
    <div class="btn" style="width:100%">
        <button onclick="location.href='/action?name=backleft'">Back Left</button>
        <button onclick="location.href='/action?name=backright'">Back Right</button>
    </div>
    <div class="btn center" style="width: 100%;">
        <button onclick="location.href='/action?name=back'">Back</button>
    </div>
    <h4 class="center">Mode</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=gyro'">GyroOn/Off</button>
        <button style="width: 100%" onclick="location.href='/action?name=calibration'">Calibration</button>
        <button style="width: 100%" onclick="location.href='/action?name=balance'">Balanced</button>
    </div>
    <h4 class="center">Actions</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=walk'">Walk</button>
        <button style="width: 100%" onclick="location.href='/action?name=trot'">Trot</button>
        <button style="width: 100%" onclick="location.href='/action?name=crawl'">Crawl</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=sit'">Sit</button>
        <button style="width: 100%" onclick="location.href='/action?name=check'">Check</button>
        <button style="width: 100%" onclick="location.href='/action?name=buttup'">Buttup</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=hello'">Hello</button>
        <button style="width: 100%" onclick="location.href='/action?name=stretch'">Stretch</button>
        <button style="width: 100%" onclick="location.href='/action?name=run'">Run</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=pee'">Pee</button>
        <button style="width: 100%" onclick="location.href='/action?name=pushup'">Push ups</button>
        <button style="width: 100%" onclick="location.href='/action?name=stepping'">Stepping</button>
    </div>
    <div class="btn-group" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=lookup'">Look up</button>
    </div>
    <div class="btn-group btn-stop" style="width:100%">
        <button style="width: 100%" onclick="location.href='/action?name=stop'">STOP</button>
    </div>
    <form action="/action" method="get">
        <div class="center" style="width: 100%; margin-top: 25px;">
            <input name="name" type="text" placeholder="input command"><input type="submit"/>
        </div>
    </form>
</div>
<div class="footer">
    <p>Petoi LLC 2021 www.petoi.com</p>
</div>
</body>

</html>

)=====";

#endif
