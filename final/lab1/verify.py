from netmiko import ConnectHandler
import sys

if len(sys.argv) < 3:
    print("python3 verify.py <MGMT address> <destination address>")
    exit(0)

cisco_device = {
    "device_type": "cisco_ios",         #OS
    "host": sys.argv[1],           #Host ip address
    "username": "cpe",                  #Username
    "password": "cpe",                  #Password
    "secret": "cpe",                    #Secret
}


connection = ConnectHandler(**cisco_device)
# Call 'enable()' method to elevate privileges
connection.enable()

# Check is enable mode
if connection.check_enable_mode():
    dest =  sys.argv[2]
    output = connection.send_command("ping " + dest)
    print(output)


connection.disconnect()
