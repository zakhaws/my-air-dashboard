// Parsing URL Parameters for Global Sharing
const urlParams = new URLSearchParams(window.location.search);
const sharedUrl = urlParams.get('url');
const sharedKey = urlParams.get('key');

// Use Shared URL/Key if present, otherwise fallback to localStorage
const configUrl = sharedUrl || localStorage.getItem('supabaseUrl');
const configKey = sharedKey || localStorage.getItem('supabaseKey');

const modal = document.getElementById('config-modal');
const statusBadge = document.getElementById('connection-status');
const dot = statusBadge.querySelector('.dot');

let supabaseClient;
const MAX_DATA_POINTS = 20;
const charts = {};

// 1. Setup Charts (Remains unchanged but called after Supabase init)
function initChart(canvasId, label, color, suggestedMax) {
    const ctx = document.getElementById(canvasId).getContext('2d');
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: label,
                data: [],
                borderColor: color,
                backgroundColor: color + '20',
                borderWidth: 2,
                tension: 0.4,
                fill: true,
                pointRadius: 0
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: { legend: { display: false } },
            scales: {
                x: { display: false },
                y: {
                    grid: { color: 'rgba(0,0,0,0.05)' },
                    ticks: { color: '#64748b' },
                    suggestedMin: 0,
                    suggestedMax: suggestedMax
                }
            },
            animation: false
        }
    });
}

function createAllCharts() {
    const ISPU_MAX = 500;
    charts.pm25 = initChart('chart-pm25', 'ISPU PM2.5', '#38bdf8', ISPU_MAX);
    charts.pm10 = initChart('chart-pm10', 'ISPU PM10', '#818cf8', ISPU_MAX);
    charts.co = initChart('chart-co', 'ISPU CO', '#f472b6', ISPU_MAX);
    charts.so2 = initChart('chart-so2', 'ISPU SO2', '#fbbf24', ISPU_MAX);
    charts.o3 = initChart('chart-o3', 'ISPU O3', '#34d399', ISPU_MAX);
    charts.no2 = initChart('chart-no2', 'ISPU NO2', '#f87171', ISPU_MAX);
    charts.temp = initChart('chart-temp', 'Suhu', '#fb923c', 50);
    charts.hum = initChart('chart-hum', 'Hum', '#22d3ee', 100);
}

// 2. Dashboard Rendering logic (similar to before, but compatible with Supabase row objects)
function updateChartData(chart, value) {
    const now = new Date().toLocaleTimeString();
    chart.data.labels.push(now);
    chart.data.datasets[0].data.push(value);
    if (chart.data.labels.length > MAX_DATA_POINTS) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
    }
    chart.update();
}

function updateDashboard(data) {
    const setText = (id, val, fixed = 0) => {
        const el = document.getElementById(id);
        if (el && val !== null && val !== undefined) el.innerText = parseFloat(val).toFixed(fixed);
    };

    const setDot = (id, val) => {
        const el = document.getElementById(id);
        if (!el) return;
        el.className = (id.includes('main')) ? 'indicator-dot-large' : 'indicator-dot-side';
        if (val <= 50) el.classList.add('bg-good');
        else if (val <= 100) el.classList.add('bg-moderate');
        else el.classList.add('bg-unhealthy');
    };

    const setISPU = (id, val, dotId1, dotId2) => {
        const el = document.getElementById(id);
        if (el && val !== null && val !== undefined) {
            val = Math.round(val);
            el.innerText = val;
            el.className = 'card-value';
            if (val <= 50) el.classList.add('c-good');
            else if (val <= 100) el.classList.add('c-moderate');
            else el.classList.add('c-unhealthy');
            if (dotId1) setDot(dotId1, val);
            if (dotId2) setDot(dotId2, val);
        }
    };

    setText('val-pm25', data.pm25, 0);
    setText('val-pm10', data.pm10, 0);
    setText('val-co', data.co, 2);
    setText('val-so2', data.so2, 2);
    setText('val-o3', data.o3, 2);
    setText('val-no2', data.no2, 2);
    setText('val-temp', data.temp, 1);
    setText('val-hum', data.hum, 1);

    setISPU('ispu-pm25', data.ispu_pm25, 'ind-ispu-pm25', 'ind-val-pm25');
    setISPU('ispu-pm10', data.ispu_pm10, 'ind-ispu-pm10', 'ind-val-pm10');
    setISPU('ispu-co', data.ispu_co, 'ind-ispu-co', 'ind-val-co');
    setISPU('ispu-so2', data.ispu_so2, 'ind-ispu-so2', 'ind-val-so2');
    setISPU('ispu-o3', data.ispu_o3, 'ind-ispu-o3', 'ind-val-o3');
    setISPU('ispu-no2', data.ispu_no2, 'ind-ispu-no2', 'ind-val-no2');

    const finalStatus = data.s_final || "UNKNOWN";
    const aqiStatus = document.getElementById('aqi-status');
    aqiStatus.innerText = finalStatus;

    const mainDot = document.getElementById('ind-main');
    aqiStatus.className = 'big-status';

    if (finalStatus === 'BAIK') {
        aqiStatus.classList.add('c-good');
        setDot('ind-main', 25); // Pass dummy value for color
    } else if (finalStatus === 'SEDANG') {
        aqiStatus.classList.add('c-moderate');
        setDot('ind-main', 75);
    } else {
        aqiStatus.classList.add('c-unhealthy');
        setDot('ind-main', 150);
    }

    document.getElementById('last-update').innerText = new Date().toLocaleTimeString();

    updateChartData(charts.pm25, data.ispu_pm25);
    updateChartData(charts.pm10, data.ispu_pm10);
    updateChartData(charts.co, data.ispu_co);
    updateChartData(charts.so2, data.ispu_so2);
    updateChartData(charts.o3, data.ispu_o3);
    updateChartData(charts.no2, data.ispu_no2);
    updateChartData(charts.temp, data.temp);
    updateChartData(charts.hum, data.hum);
}

// 3. Sharing & Config Utility
function shareCurrentConfig() {
    const url = localStorage.getItem('supabaseUrl');
    const key = localStorage.getItem('supabaseKey');
    if (!url || !key) {
        alert("Atur konfigurasi terlebih dahulu sebelum berbagi!");
        return;
    }
    const shareUrl = `${window.location.origin}${window.location.pathname}?url=${encodeURIComponent(url)}&key=${encodeURIComponent(key)}`;
    navigator.clipboard.writeText(shareUrl).then(() => {
        alert("Link dashboard telah disalin ke clipboard! Bagikan link ini agar orang lain bisa melihat data Anda secara global.");
    });
}

function clearConfig() {
    localStorage.removeItem('supabaseUrl');
    localStorage.removeItem('supabaseKey');
    window.location.href = window.location.pathname; // Reload without params
}

// 4. Initialization Logic
if (!configUrl || !configKey) {
    modal.style.display = 'flex';
} else {
    initSupabase(configUrl, configKey);
}

document.getElementById('save-config').addEventListener('click', () => {
    const url = document.getElementById('supabase-url').value.trim();
    const key = document.getElementById('supabase-key').value.trim();
    if (url && key) {
        localStorage.setItem('supabaseUrl', url);
        localStorage.setItem('supabaseKey', key);
        window.location.reload();
    } else {
        alert("Mohon masukkan URL dan Key Supabase Anda.");
    }
});

function initSupabase(url, key) {
    const { createClient } = supabase;
    try {
        supabaseClient = createClient(url, key);
        statusBadge.innerHTML = '<span class="dot"></span> Terhubung (Global)';
        statusBadge.classList.add('connected');
        createAllCharts();

        // 1. Fetch Latest Data
        supabaseClient
            .from('sensors')
            .select('*')
            .order('created_at', { ascending: false })
            .limit(1)
            .then(({ data, error }) => {
                if (data && data.length > 0) updateDashboard(data[0]);
            });

        // 2. Subscribe to Realtime Updates
        supabaseClient
            .channel('public:sensors')
            .on('postgres_changes', { event: 'INSERT', schema: 'public', table: 'sensors' }, payload => {
                updateDashboard(payload.new);
            })
            .on('postgres_changes', { event: 'UPDATE', schema: 'public', table: 'sensors' }, payload => {
                updateDashboard(payload.new);
            })
            .subscribe();

    } catch (e) {
        console.error(e);
        statusBadge.innerHTML = 'Error Koneksi';
    }
}

// Download CSV Implementation
function downloadData() {
    const labels = charts.pm25.data.labels;
    if (!labels || labels.length === 0) {
        alert("Belum ada data untuk di-download!");
        return;
    }
    let csvContent = "data:text/csv;charset=utf-8,Waktu,PM2.5,PM10,CO,SO2,O3,NO2,Suhu,Kelembapan\n";
    labels.forEach((time, index) => {
        csvContent += [
            time,
            charts.pm25.data.datasets[0].data[index],
            charts.pm10.data.datasets[0].data[index],
            charts.co.data.datasets[0].data[index],
            charts.so2.data.datasets[0].data[index],
            charts.o3.data.datasets[0].data[index],
            charts.no2.data.datasets[0].data[index],
            charts.temp.data.datasets[0].data[index],
            charts.hum.data.datasets[0].data[index]
        ].join(",") + "\n";
    });
    const link = document.createElement("a");
    link.href = encodeURI(csvContent);
    link.download = `data_sensor_global_${new Date().toISOString().slice(0, 10)}.csv`;
    link.click();
}
