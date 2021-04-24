#ifndef COMMONS_H
#define COMMONS_H

const char head[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>OpenCAT Nybble Web Control</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=0.5, maximum-scale=0.8, user-scalable=yes"/>
    <link rel="stylesheet" href="https://cdn.staticfile.org/twitter-bootstrap/4.3.1/css/bootstrap.min.css">

    <style>
        .header {
            background-color: #F1F1F1;
            text-align: center;
        }

        h1 {
            padding: 10px;
        }

        h4 {
          margin-top: 15px;
        }

        ul {
            list-style-type: none;
            margin: 0;
            padding: 0;
            overflow: hidden;
            border: 1px solid #E7E7E7;
            background-color: #F1F1F1;
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
            margin: 5px 5px 10px;
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

        .content {
            margin: 25px;
        }

        .explanation {
            width: 50%;
            margin: 15px auto;
            background-color: #7dc280;
            border: 1px solid black;
            padding: 15px;
            box-sizing: border-box;
            border-radius: 20px;
        }
    </style>
    <script type="text/javascript">
        document.addEventListener('DOMContentLoaded', function () {
            let title = "%TITLE%";
            document.getElementById(title).classList.add('active')
        });
    </script>
</head>

<body>
<div class="header">
    <ul>
        <li><a id="Home" href="/">Home</a></li>
        <li><a id="Actions" href="/actionpage">Actions</a></li>
        <li><a id="Calibration" href="/calibrationpage">Calibration</a></li>
    </ul>
    <h1>%TITLE%</h1>
</div>
)=====";

const char foot[] PROGMEM = R"=====(
<div class="footer">
    <p>Petoi LLC 2021 www.petoi.com</p>
</div>
</body>
</html>
)=====";

#endif
