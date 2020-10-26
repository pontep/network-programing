from netmiko import ConnectHandler

cisco_device = {
    'device_type': 'cisco_ios',
    'host':   '192.168.254.23',
    'username': 'cpe',
    'password': 'cpe',         # optional, defaults to 22
    'secret': 'cpe',     # optional, defaults to ''
}

connection = ConnectHandler(**cisco_device)

# Enter enable() mode
connection.enable()

if connection.check_enable_mode():

    # Show hostname
    prompt = connection.find_prompt()
    print("#Verify hostname\n")
    print(prompt)
    # Show interface
    hostname = connection.send_command("show ip int br")
    print("#Verify interfaces\n")
    print(hostname)
    # Show routes
    routes = connection.send_command("show ip route")
    print("#Verify routes\n")
    print(routes)

connection.disconnect()


# Enter config() mode
