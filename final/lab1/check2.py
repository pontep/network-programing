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
    # Enter config mode
    connection.config_mode()

    # Check if in config mode
    if connection.check_config_mode():
        # config
        command = ["hostname C-SW-5", "int g1/0/24", "no sw", "ip address 172.58.10.6 255.255.255.252", "exit", "ip routing",
                   "ip route 100.254.10.4 255.255.255.252 172.58.10.5", "ip route 100.254.10.0 255.255.255.252 172.58.10.5", "ip route 172.58.10.0 255.255.255.252 172.58.10.5"]
        connection.send_config_set(config_commands=command)

    # Verify
    # Show hostname
    prompt = connection.find_prompt()
    print("#Verify hostname\n")
    print(prompt)
    # Show routes
    routes = connection.send_command("show ip route")
    print("#Verify routes\n")
    print(routes)

connection.disconnect()


# Enter config() mode
