from nornir import InitNornir
from nornir_utils.plugins.functions import print_result
from nornir_jinja2.plugins.tasks import template_file
from nornir_napalm.plugins.tasks import napalm_get
from nornir_netmiko.tasks import netmiko_send_config, netmiko_send_command
nr = InitNornir(config_file="config.yaml")  # Call config.yaml


def turnon_iprouting(task):
    r = task.run(
        task=template_file,
        name='Generate configuration for enable ip routing',
        template='iprouting.j2',
        path=f'template'
    )
    config = r.result
    task.run(
        task=netmiko_send_config,
        name='Send config',
        config_commands=config.splitlines()
    )


def assign_ip(task):
    r = task.run(
        task=template_file,
        name='Generate configuration for assign ip',
        template='assignip.j2',
        path=f'template'
    )
    config = r.result
    task.run(
        task=netmiko_send_config,
        name='Send config',
        config_commands=config.splitlines()
    )


def assign_iproute(task):
    r = task.run(
        task=template_file,
        name='Generate configuration for assign ip route',
        template='iproute.j2',
        path=f'template'
    )
    config = r.result
    task.run(
        task=netmiko_send_config,
        name='Send config',
        config_commands=config.splitlines()
    )


if __name__ == '__main__':
    # assign_ip
    result = nr.run(task=assign_ip)
    print_result(result)

    # enable ip routing
    result = nr.run(task=turnon_iprouting)
    print_result(result)

    # assign ip route
    result = nr.run(task=assign_iproute)
    print_result(result)

    #
    shInterfaces = nr.run(task=napalm_get, getters=['interfaces_ip'])
    print_result(shInterfaces)
    shIpRoute = nr.run(task=netmiko_send_command, command_string='sh ip route')
    print_result(shIpRoute)
