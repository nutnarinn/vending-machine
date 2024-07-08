import serial
import requests
import urllib
import re

ser = serial.Serial('/dev/ttyS1', 9600)  # Arduino's serial port

# Line Notify API settings
line_notify_token = 'TP1owLq7eIENQc7TDekX8KvK8tM4KodIxq3yKydEr0b'
line_notify_url = 'https://notify-api.line.me/api/notify'

# Thingspeak
baseURL= 'https://api.thingspeak.com/update?api_key=RIGSS8OXBIKPRC2R'

headers = {
    'Authorization': f'Bearer {line_notify_token}',
}

def send_line_notify(message):
    data = {
        'message': message,
    }
    response = requests.post(line_notify_url, headers=headers, data=data)
    if response.status_code == 200:
        print(f'Message sent to Line Notify: {message}')
    else:
        print(f'Failed to send message to Line Notify')
    print(response.text)

def update_field(url, field, value):
    try:
        f = urllib.request.urlopen(url + f'&{field}={value}')
        f.read()
        f.close()
        print(f'Send URL request with {field}: {value}')
    except Exception as e:
        print(f'Error updating {field}: {str(e)}')

while True:
    try:
        data = ser.readline().decode('utf-8').strip()
        
        # Send each line received from Arduino to Line Notify
        if "remaining" in data or "stock" in data or "Total income" in data:
            send_line_notify(data)
        
        match = re.search(r'Total income:\s*(\d+)', data)
        if match:
            income_total = match.group(1)
            print(income_total)
            update_field(baseURL, 'field1', income_total)
        else:
            pass
            
    except Exception as e:
        print(f'Error: {str(e)}')