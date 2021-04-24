#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

const char calibrationpage[] PROGMEM = R"=====(
<div class="content">
    <div class="explanation">
        <h5>Calibrating is vital for your robot to correctly stand and walk</h5>
        <p>It is important to understand that the servos installed on your robot do have some backlash and that you have to overcome it.</p>
        <p><b>Do calibrate all your joints on the same direction!</b> What does this mean in practice? Well, choose if you are going to fine tune with "+" buttons or "-" buttons. Let's say you chose the "+" button. First, exaggerate the "-" offset, so that the joint is totally off calibration. Then, slowly come back to calibration position with the "+" button. If you pass the correct angle, <b>do not just step back one offset</b> or you'll see that the servo might not even move! Just exaggerate the "-" offset again and slowly come back with the "+". Choose the same direction for all joints!</p>
        <p><b>Don't forget to hit the save button once you're done, or you'll loose your hard work!</b></p>
    </div>
    <div class="center">
        <img src="https://gblobscdn.gitbook.com/assets%2F-MPQ2vWEZUH7ol6XE55o%2F-MShb_2_2fVamKuokK2A%2F-MShe8Oogbo5SCF0YULN%2FIndex.png?alt=media&token=ca8cc841-460a-4f10-af02-aa9d73093a0e" alt="Bittle joint overview" width="300">
    </div>
    <h4>Offset amount</h4>
    <div class="btn-group center" style="width:100%">
        <input id="offsetAmountSetter" type="range" min="1" max="10" step="1" value="1" name="offsetAmount" onchange="setOffset(this.value)"/>
        <span id="offsetAmount">1</span>
    </div>
    <h4>Head (0)</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 50%" onclick="increase(0)">Left</button>
        <button style="width: 50%" onclick="decrease(0)">Right</button>
    </div>
    <h4>Front left leg</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 25%" onclick="increase(8)">(8) Upper joint +</button>
        <button style="width: 25%" onclick="decrease(8)">(8) Upper joint -</button>
        <button style="width: 25%" onclick="increase(12)">(12) Lower joint +</button>
        <button style="width: 25%" onclick="decrease(12)">(12) Lower joint -</button>
    </div>
    <h4>Front right leg</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 25%" onclick="increase(9)">(9) Upper joint +</button>
        <button style="width: 25%" onclick="decrease(9)">(9) Upper joint -</button>
        <button style="width: 25%" onclick="increase(13)">(13) Lower joint +</button>
        <button style="width: 25%" onclick="decrease(13)">(13) Lower joint -</button>
    </div>
    <h4>Back left leg</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 25%" onclick="increase(11)">(11) Upper joint +</button>
        <button style="width: 25%" onclick="decrease(11)">(11) Upper joint -</button>
        <button style="width: 25%" onclick="increase(15)">(15) Lower joint +</button>
        <button style="width: 25%" onclick="decrease(15)">(15) Lower joint -</button>
    </div>
    <h4>Front right leg</h4>
    <div class="btn-group" style="width:100%">
        <button style="width: 25%" onclick="increase(10)">(10) Upper joint +</button>
        <button style="width: 25%" onclick="decrease(10)">(10) Upper joint -</button>
        <button style="width: 25%" onclick="increase(14)">(14) Lower joint +</button>
        <button style="width: 25%" onclick="decrease(14)">(14) Lower joint -</button>
    </div>
    <div class="btn center" style="width: 100%; margin-top: 25px">
        <button onclick="location.href='/calibration?c=s'">Save offsets</button>
    </div>
    <script type="text/javascript">
        document.addEventListener('DOMContentLoaded', function() {
            let query = window.location.search;
            const urlParams = new URLSearchParams(query);
            if (urlParams.has('o')) {
                let offset = Math.max(1, Math.min(Math.abs(parseInt(urlParams.get('o'))) - 1000, 10));
                document.getElementById('offsetAmountSetter').value = offset;
                setOffset(offset);
            }
        });
  
        function setOffset(value) {
            document.getElementById('offsetAmount').innerHTML = value;
        }
  
        function increase(joint) {
            window.location.href=`/calibration?c=${joint}&o=${1000 + parseInt(document.getElementById('offsetAmountSetter').value)}`;
        }
  
        function decrease(joint) {
            window.location.href=`/calibration?c=${joint}&o=${-1000 - parseInt(document.getElementById('offsetAmountSetter').value)}`;
        }
    </script>
</div>
)=====";

#endif
