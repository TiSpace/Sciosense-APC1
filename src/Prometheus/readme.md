This is a basic example for an APC1 with UART interface, gathering data and provide for Prometheus database.
Obciously Prometheus has to be setup accordingly.

Example derived from (HighOnVoltageEngineer)[https://github.com/highonvoltageen/Sciosense-APC1-ESP32/tree/main]<br>
You need to update ```prometheus.yml``` in the scrape configuration to reflect<br>
```
 - job_name: "APC1"
    scrape_interval: 500ms 
    static_configs: 
    - targets: ["192.168.178.141"]
```
where the target address of course need to be the one of your controller
