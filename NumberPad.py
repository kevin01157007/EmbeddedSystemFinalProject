import network
import time
import config
from umqtt.simple import MQTTClient
from machine import UART
import xtools

# 初始化 UART2，TX:17, RX:16
com = UART(2, 9600, tx=17, rx=16)
com.init(9600)

# MQTT 基本設定
ADAFRUIT_IO_URL = "io.adafruit.com"
ADAFRUIT_IO_USERNAME = config.ADAFRUIT_IO_USERNAME
ADAFRUIT_IO_KEY = config.ADAFRUIT_IO_KEY
FEED_NAME = "number"
MQTT_TOPIC = bytes(f"{ADAFRUIT_IO_USERNAME}/feeds/{FEED_NAME}", "utf-8")

# 連上 Wi-Fi
xtools.connect_wifi_led()

# MQTT 回呼函數
def message_callback(topic, msg):
    num = msg.decode().strip()
    print("收到 MQTT 數字：", num)

    # 傳給 8051（轉成字串傳送）
    if num == "*":
        com.write(b'SET\r\r')     # 特殊按鍵 SET
    elif num == "#":
        com.write(b'ENTER')   # 特殊按鍵 ENTER
    elif num.isdigit():
        com.write(num.encode() + b'\r\r\r\r')  # 一般數字按鍵
    else:
        print("未知按鍵:", num)

# 建立 MQTT 客戶端
client = MQTTClient(
    client_id="esp32_lock",
    server=ADAFRUIT_IO_URL,
    user=ADAFRUIT_IO_USERNAME,
    password=ADAFRUIT_IO_KEY,
    ssl=False
)
client.set_callback(message_callback)
client.connect()
client.subscribe(MQTT_TOPIC)
print("✅ MQTT 已連接並訂閱：", MQTT_TOPIC)

# 主迴圈
try:
    while True:
        client.check_msg()  # 接收 MQTT 訊息


        if com.any() > 0:
            data = com.readline()
            if data:
                print("📥 從 8051 收到:", data.decode().strip())

        time.sleep(0.1)

except KeyboardInterrupt:
    client.disconnect()
    print("❌ MQTT 斷線")
