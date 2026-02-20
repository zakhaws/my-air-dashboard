#ifndef DASHBOARD_H
#define DASHBOARD_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Air Quality Monitor</title>
  <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600&display=swap" rel="stylesheet">
  <style>
    :root {
      --bg-color: #121212;
      --card-bg: #1e1e1e;
      --text-main: #e0e0e0;
      --text-muted: #a0a0a0;
      --accent: #00d4ff;
      --good: #00e676;
      --moderate: #ffea00;
      --unhealthy: #ff3d00;
      --glass: rgba(255, 255, 255, 0.05);
      --border: rgba(255, 255, 255, 0.1);
    }

    body {
      font-family: 'Outfit', sans-serif;
      background-color: var(--bg-color);
      color: var(--text-main);
      margin: 0;
      padding: 20px;
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
    }

    h1 {
      margin-bottom: 5px;
      font-weight: 600;
      letter-spacing: 1px;
      text-transform: uppercase;
      background: linear-gradient(90deg, var(--accent), #00ff9d);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }

    p.subtitle {
      color: var(--text-muted);
      margin-top: 0;
      margin-bottom: 30px;
      font-size: 0.9rem;
    }

    .container {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
      gap: 20px;
      width: 100%;
      max-width: 1000px;
    }

    .card {
      background: var(--card-bg);
      border-radius: 16px;
      padding: 20px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      box-shadow: 0 4px 6px rgba(0,0,0,0.3);
      border: 1px solid var(--border);
      transition: transform 0.2s, box-shadow 0.2s;
      position: relative;
      overflow: hidden;
    }

    .card:hover {
      transform: translateY(-5px);
      box-shadow: 0 8px 12px rgba(0,0,0,0.5);
      border-color: rgba(255, 255, 255, 0.2);
    }

    .card::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 4px;
      background: var(--glass);
    }

    .card.good::before { background: var(--good); }
    .card.moderate::before { background: var(--moderate); }
    .card.unhealthy::before { background: var(--unhealthy); }

    .label {
      font-size: 0.85rem;
      color: var(--text-muted);
      margin-bottom: 10px;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }

    .value {
      font-size: 2.5rem;
      font-weight: 600;
      margin-bottom: 5px;
    }

    .unit {
      font-size: 0.9rem;
      color: var(--text-muted);
      vertical-align: super;
    }

    .status {
      font-size: 0.8rem;
      padding: 4px 12px;
      border-radius: 20px;
      background: var(--glass);
      margin-top: 10px;
      font-weight: 600;
    }

    .status.good { color: var(--good); border: 1px solid rgba(0, 230, 118, 0.2); }
    .status.moderate { color: var(--moderate); border: 1px solid rgba(255, 234, 0, 0.2); }
    .status.unhealthy { color: var(--unhealthy); border: 1px solid rgba(255, 61, 0, 0.2); }

    /* Main Status Card */
    .card.main {
      grid-column: 1 / -1;
      background: linear-gradient(135deg, rgba(255,255,255,0.05) 0%, rgba(255,255,255,0.02) 100%);
      flex-direction: row;
      justify-content: space-around;
      padding: 30px;
      min-height: 120px;
    }

    .main-status-text {
      text-align: center;
    }
    
    .main-status-label {
      display: block;
      font-size: 1rem;
      color: var(--text-muted);
      margin-bottom: 5px;
    }

    .main-status-value {
      font-size: 2rem;
      font-weight: 700;
      letter-spacing: 1px;
    }

    @media (max-width: 600px) {
      .card.main {
        flex-direction: column;
        gap: 15px;
        text-align: center;
      }
      .value { font-size: 2rem; }
    }
  </style>
</head>
<body>

  <h1>Air Quality Monitor</h1>
  <p class="subtitle">Real-time Environmental Monitoring</p>

  <div class="container">
    
    <!-- Overall Status -->
    <div class="card main" id="card-main">
      <div class="main-status-text">
        <span class="main-status-label">Overall Quality</span>
        <span id="aq-status" class="main-status-value">LOADING...</span>
      </div>
      <div class="main-status-text">
        <span class="main-status-label">Max ISPU</span>
        <span id="max-ispu">--</span>
      </div>
    </div>

    <!-- PM2.5 -->
    <div class="card" id="card-pm25">
      <div class="label">PM2.5</div>
      <div class="value"><span id="pm25">--</span></div>
      <div class="unit">ISPU: <span id="ispu-pm25">--</span></div>
      <div class="status" id="status-pm25">--</div>
    </div>

    <!-- PM10 -->
    <div class="card" id="card-pm10">
      <div class="label">PM10</div>
      <div class="value"><span id="pm10">--</span></div>
      <div class="unit">ISPU: <span id="ispu-pm10">--</span></div>
      <div class="status" id="status-pm10">--</div>
    </div>

    <!-- CO -->
    <div class="card" id="card-co">
      <div class="label">CO</div>
      <div class="value"><span id="co">--</span></div>
      <div class="unit">ISPU: <span id="ispu-co">--</span></div>
      <div class="status" id="status-co">--</div>
    </div>

    <!-- SO2 -->
    <div class="card" id="card-so2">
      <div class="label">SO2</div>
      <div class="value"><span id="so2">--</span></div>
      <div class="unit">ISPU: <span id="ispu-so2">--</span></div>
      <div class="status" id="status-so2">--</div>
    </div>

    <!-- O3 -->
    <div class="card" id="card-o3">
      <div class="label">O3</div>
      <div class="value"><span id="o3">--</span></div>
      <div class="unit">ISPU: <span id="ispu-o3">--</span></div>
      <div class="status" id="status-o3">--</div>
    </div>

    <!-- NO2 -->
    <div class="card" id="card-no2">
      <div class="label">NO2</div>
      <div class="value"><span id="no2">--</span></div>
      <div class="unit">ISPU: <span id="ispu-no2">--</span></div>
      <div class="status" id="status-no2">--</div>
    </div>

    <!-- Temp -->
    <div class="card">
      <div class="label">Temperature</div>
      <div class="value"><span id="temp">--</span><span class="unit">°C</span></div>
    </div>

    <!-- Humidity -->
    <div class="card">
      <div class="label">Humidity</div>
      <div class="value"><span id="hum">--</span><span class="unit">%</span></div>
    </div>

  </div>

  <script>
    function updateStatusClass(elementId, status) {
      const el = document.getElementById(elementId);
      el.classList.remove('good', 'moderate', 'unhealthy');
      if (status === 'BAIK') el.classList.add('good');
      else if (status === 'SEDANG') el.classList.add('moderate');
      else el.classList.add('unhealthy');
    }

    function fetchData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          // Values
          document.getElementById('pm25').innerText = data.pm25;
          document.getElementById('pm10').innerText = data.pm10;
          document.getElementById('co').innerText = parseFloat(data.co).toFixed(1);
          document.getElementById('so2').innerText = parseFloat(data.so2).toFixed(1);
          document.getElementById('o3').innerText = parseFloat(data.o3).toFixed(1);
          document.getElementById('no2').innerText = parseFloat(data.no2).toFixed(1);
          
          // ISPU
          document.getElementById('ispu-pm25').innerText = Math.round(data.ispu_pm25);
          document.getElementById('ispu-pm10').innerText = Math.round(data.ispu_pm10);
          document.getElementById('ispu-co').innerText = Math.round(data.ispu_co);
          document.getElementById('ispu-so2').innerText = Math.round(data.ispu_so2);
          document.getElementById('ispu-o3').innerText = Math.round(data.ispu_o3);
          document.getElementById('ispu-no2').innerText = Math.round(data.ispu_no2);

          // Temp & Hum
          document.getElementById('temp').innerText = parseFloat(data.temp).toFixed(1);
          document.getElementById('hum').innerText = parseFloat(data.hum).toFixed(1);

          // Status Strings
          document.getElementById('status-pm25').innerText = data.s_pm25;
          document.getElementById('status-pm10').innerText = data.s_pm10;
          document.getElementById('status-co').innerText = data.s_co;
          document.getElementById('status-so2').innerText = data.s_so2;
          document.getElementById('status-o3').innerText = data.s_o3;
          document.getElementById('status-no2').innerText = data.s_no2;
          
          document.getElementById('aq-status').innerText = data.s_final;
          document.getElementById('max-ispu').innerText = Math.round(data.ispu_max);

          // Update Colors
          updateStatusClass('card-pm25', data.s_pm25);
          updateStatusClass('card-pm10', data.s_pm10);
          updateStatusClass('card-co', data.s_co);
          updateStatusClass('card-so2', data.s_so2);
          updateStatusClass('card-o3', data.s_o3);
          updateStatusClass('card-no2', data.s_no2);
          updateStatusClass('status-pm25', data.s_pm25);
          updateStatusClass('status-pm10', data.s_pm10);
          updateStatusClass('status-co', data.s_co);
          updateStatusClass('status-so2', data.s_so2);
          updateStatusClass('status-o3', data.s_o3);
          updateStatusClass('status-no2', data.s_no2);
          
          // Main Card Color
          const mainCard = document.getElementById('card-main');
          mainCard.classList.remove('good', 'moderate', 'unhealthy');
          const finalStatus = document.getElementById('aq-status');
          finalStatus.style.color = 'var(--text-main)';
          
          if (data.s_final === 'BAIK') {
            mainCard.classList.add('good');
            finalStatus.style.color = 'var(--good)';
          } else if (data.s_final === 'SEDANG') {
            mainCard.classList.add('moderate');
            finalStatus.style.color = 'var(--moderate)';
          } else {
            mainCard.classList.add('unhealthy');
            finalStatus.style.color = 'var(--unhealthy)';
          }
        })
        .catch(error => console.error('Error fetching data:', error));
    }

    // Fetch initial data
    fetchData();
    // Update every 2 seconds
    setInterval(fetchData, 2000);
  </script>
</body>
</html>
)rawliteral";

#endif
