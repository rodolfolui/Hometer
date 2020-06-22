/*  Copyright (C) 2020 Rodolfo Vasquez

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

// Watson IoT connection details
#define MQTT_HOST "v45txk.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:v45txk:NodeMCUESP8266:Hometer01"
//#define MQTT_DEVICEID "d:v45txk:NodeMCUESP8266:Hometer02"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "ig@mnla)75JO!Bh-!2"
//#define MQTT_TOKEN "JwPxSEkjYXk33yTem1"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"
