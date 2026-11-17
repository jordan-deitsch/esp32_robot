#pragma once
// Single-page control UI, served from flash.
static const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Robot</title>
<style>
 :root{--bg:#111827;--card:#1f2937;--fg:#f3f4f6;--mut:#9ca3af;--on:#16a34a;--off:#4b5563;--acc:#2563eb}
 *{box-sizing:border-box}
 body{margin:0;font-family:system-ui,Arial,sans-serif;background:var(--bg);color:var(--fg);padding:16px;max-width:760px;margin:auto}
 header{display:flex;justify-content:space-between;align-items:center;margin-bottom:12px}
 h1{font-size:1.3rem;margin:0}
 h2{font-size:1.05rem;margin:0 0 10px}
 .card{background:var(--card);border-radius:12px;padding:14px;margin-bottom:12px}
 button{border:0;border-radius:8px;padding:10px 14px;font-size:1rem;color:#fff;background:var(--off);cursor:pointer}
 button.active{background:var(--acc)}
 button.power{width:100%;font-weight:600;margin-bottom:10px}
 button.power.on{background:var(--on)}
 button.stop{background:#dc2626;font-weight:700}
 .row{display:flex;gap:8px;margin-bottom:10px}
 .row button{flex:1}
 input[type=range]{width:100%;margin:6px 0 8px}
 small,label{color:var(--mut)}
 b{color:var(--fg)}
 #conn{font-size:.85rem;color:var(--mut)}
</style></head><body>
<header><h1>ESP32 Robot</h1><span id="conn">connecting…</span></header>
<div class="card"><button class="stop power" onclick="api('/api/stopall')">⏹ STOP ALL</button></div>
<div id="steppers"></div>
<div id="servo"></div>
<script>
const $ = id => document.getElementById(id);
let state = null, built = false, lastServo = 0;

async function api(path){
  try{
    const r = await fetch(path, {cache:'no-store'});
    state = await r.json();
    $('conn').textContent = 'connected';
    if(!built) build();
    render();
  }catch(e){ $('conn').textContent = 'offline'; }
}

function build(){
  built = true;
  state.steppers.forEach(s=>{
    const d = document.createElement('div'); d.className='card';
    d.innerHTML = `<h2>${s.name}</h2>
      <button class="power" id="pw${s.id}"></button>
      <div class="row"><button id="cw${s.id}">⟳ Clockwise</button><button id="ccw${s.id}">⟲ Counter-CW</button></div>
      <label>Speed: <b id="sv${s.id}"></b> steps/s</label>
      <input type="range" id="sp${s.id}" min="1" max="${s.max}">
      <small>Position: <span id="ps${s.id}">0</span> steps</small>`;
    $('steppers').appendChild(d);
    const cur = () => state.steppers[s.id];
    $('pw'+s.id).onclick  = () => api(`/api/stepper?id=${s.id}&run=${cur().running?0:1}`);
    $('cw'+s.id).onclick  = () => api(`/api/stepper?id=${s.id}&dir=cw`);
    $('ccw'+s.id).onclick = () => api(`/api/stepper?id=${s.id}&dir=ccw`);
    $('sp'+s.id).oninput  = e => $('sv'+s.id).textContent = e.target.value;
    $('sp'+s.id).onchange = e => api(`/api/stepper?id=${s.id}&speed=${e.target.value}`);
  });
  const v = document.createElement('div'); v.className='card';
  v.innerHTML = `<h2>Servo</h2>
    <button class="power" id="svpw"></button>
    <label>Angle: <b id="sva"></b>°</label>
    <input type="range" id="svs" min="${state.servo.min}" max="${state.servo.max}">
    <div class="row"><button id="a0">${state.servo.min}°</button><button id="a90">90°</button><button id="a180">${state.servo.max}°</button></div>`;
  $('servo').appendChild(v);
  $('svpw').onclick = () => api(`/api/servo?enabled=${state.servo.enabled?0:1}`);
  $('svs').oninput = e => {
    $('sva').textContent = e.target.value;
    const now = Date.now();
    if(now - lastServo > 80){ lastServo = now; api(`/api/servo?angle=${e.target.value}`); }
  };
  $('svs').onchange = e => api(`/api/servo?angle=${e.target.value}`);
  [[0,state.servo.min],[90,90],[180,state.servo.max]].forEach(([id,a]) => $('a'+id).onclick = () => api(`/api/servo?angle=${a}`));
}

function render(){
  state.steppers.forEach(s=>{
    const pw = $('pw'+s.id);
    pw.textContent = s.running ? 'RUNNING – tap to stop' : 'STOPPED – tap to start';
    pw.className = 'power' + (s.running ? ' on' : '');
    $('cw'+s.id).className  = s.cw ? 'active' : '';
    $('ccw'+s.id).className = s.cw ? '' : 'active';
    $('ps'+s.id).textContent = s.pos;
    if(document.activeElement !== $('sp'+s.id)){
      $('sp'+s.id).value = s.speed;
      $('sv'+s.id).textContent = s.speed;
    }
  });
  const sv = state.servo, pw = $('svpw');
  pw.textContent = sv.enabled ? 'ENABLED – tap to release' : 'DISABLED – tap to enable';
  pw.className = 'power' + (sv.enabled ? ' on' : '');
  if(document.activeElement !== $('svs')){
    $('svs').value = sv.angle;
    $('sva').textContent = sv.angle;
  }
}

api('/api/status');
setInterval(() => api('/api/status'), 1500);
</script></body></html>)rawliteral";
