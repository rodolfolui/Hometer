/*  Copyright (C) 2016 Buxtronix and Alexander Pruss

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

const char MAIN_page[] PROGMEM = R"=====(
<html>
<head>
 <meta name="viewport" content="initial-scale=1">
 <style>
 body {font-family: helvetica,arial,sans-serif;}
 table {border-collapse: collapse; border: 1px solid black;}
 td {padding: 0.25em;}
 .title { font-size: 2em; font-weight: bold;}
 .name {padding: 0.5em;}
 .heading {font-weight: bold; background: #c0c0c0; padding: 0.5em;}
 .update {color: #dd3333; font-size: 0.75em;}
 </style>
</head>
<div class=title>IoT Consumo</div>
<form method="post" action="/form">
<table>
<tr><td colspan=2 class=heading>Estado</td></tr>
<tr><td>Hora actual:</td><td><b>@@HOUR@@:@@MIN@@</b> @@SYNCSTATUS@@</td></tr>
<tr><td>Potencia:</td><td>@@POTEN@@ W</td></tr>
<tr><td>Irms:</td><td>@@IRMS@@ A</td></tr>
<tr><td>Humedad:</td><td>@@HUMDTY@@ %</td></tr>
<tr><td>Temperatura:</td><td>@@TEMP@@ &#186C</td></tr>
<tr><td>HIC:</td><td>@@HIC@@ &#186C</td></tr>
<tr><td>Name:</td><td><input type=text name="clockname" value="@@CLOCKNAME@@"></td></tr>

<tr><td colspan=2 class=heading>Configuracion WiFi</td></tr>
<tr><td>SSID:</td><td><input type=text name="ssid" value="@@SSID@@"></td></tr>
<tr><td>PSK:</td><td><input type=password name="psk" value="@@PSK@@"></td></tr>
<tr><td colspan=2>Actualizar config. Wifi:<input type=checkbox name=update_wifi value=1></td></tr>

<tr><td colspan=2 class=heading>Configuracion reloj</td></tr>
<tr><td>Servidor NTP:</td><td><input type=text name="ntpsrv" value="@@NTPSRV@@"></td></tr>
<tr><td>Sincronia:</td><td><input type=text name="ntpint" value="@@NTPINT@@">s</td></tr>
<tr><td>Zona horaria:</td><td><input type=text name="timezone" value="@@TZ@@">GMT</td></tr>
</table>
<p/>
<input type="submit" value="Update">
</form>
<div class="update">@@UPDATERESPONSE@@</div>
</html>
)=====";
