# CINNOXtest

兩個檔案分別是server跟client<br />
server有特別寫讓他一直在listen的狀態<br />
client則是單次使用，但是加入retry的功能<br />
影片錄製重要功能：server有正常echo message，以及client在回應超時的時候會有限度的嘗試重新連線<br />
影片環境是用docker跑debian:10的映像檔<br />


