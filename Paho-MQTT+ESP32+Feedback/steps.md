sudo mosquitto_passwd -c /etc/mosquitto/passwd prateek

sudo nano /etc/mosquitto/conf.d/custom.conf

listener 1883
allow_anonymous false
password_file /etc/mosquitto/passwd

sudo systemctl restart mosquitto


mosquitto_pub -h localhost -t test -m "hello" -u prateek -P 1997
mosquitto_sub -h localhost -t test  -u prateek -P 1997 -d







