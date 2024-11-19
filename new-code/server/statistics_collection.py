mac_addresses = ["f4ce.36da.a6f8.362b", "f4ce.363c.bf2b.c116", "f4ce.36f6.4782.666f", "f4ce.36d4.a436.87a8", "f4ce.3695.a196.a1e5", "f4ce.3616.55de.d3a3"]
coordinator = mac_addresses[0]

all_statistics = [{"src_addr":addr, "l_stats_of_neighbours":[]} for addr in mac_addresses]

no_addr_addr = "4444.4444.4444.4444"

def tsch_send(serial_connection, slotframe, timeslot, channeloffset, addr_1, addr_2):
    """
    Sends tsch_send message to node (which sends the tsch_message into the network) to create/change/delete a slotfram or timeslot.
    :param serial_connection: Serial connection for writing the message to the node
    :param slotframe: Slotframe to be created / changed / deleted
    :param timeslot: Timeslot in slotframe to be created / changed / deleted
    :param channeloffset: Channeloffset in timeslot 
    :param addr_1: Mac address of node 1
    :param addr_2: Mac address of node 2
    :return: None
    """
    
    msg = f"tsch-send {slotframe} {timeslot} {channeloffset} {addr_1} {addr_2}\n"
    serial_connection.write(bytes(msg, "utf-8"))
    serial_connection.flush()

def request_stats(serial_connection, addr, all_stats):
    """
    Sends request_stats message to node (which sends the request_message into the network) for requesting statistics form node(s). 
    :param serial_connection: Serial connection for writing the message to the node
    :param addr: Mac address of the node, from which the statistics is requested
    :param all_stats: 0 or 1, requesting the stats of ``addr`` from nodes where addr is the neighbour or requesting all stats from ``addr``
    :return: None
    """
    msg = f"request-stats {addr} {all_stats}\n"
    serial_connection.write(bytes(msg, "utf-8"))
    serial_connection.flush()

    count = 0
    while count < 20:
        x = serial_connection.readline()
        y = x.decode("utf-8").replace("\n", "")
        content = y.split(" ")
        if len(content) == 2 and content[0] == "1":
            parse_answers_all_statistics(serial_connection)
        count += 1


def parse_answers_all_statistics(serial_connection):
    """
    Parses answers from request_message
    :param serial_connection: Serial connection to read from
    :return: None
    """
    src_addr_new = serial_connection.readline()
    src_addr = src_addr_new.decode("utf-8").replace("\n", "")

    mac_index = mac_addresses.index(src_addr)

    next_line = serial_connection.readline()
    neighbor1_stats_all = next_line.decode("utf-8").replace("\n", "")
    neighbor1_stats_list = neighbor1_stats_all.split(" ")

    if neighbor1_stats_list[0] != no_addr_addr:
        stats1 = {"addr": neighbor1_stats_list[0], "last_tx_time": neighbor1_stats_list[1], "etx": neighbor1_stats_list[2],
                  "rssi": neighbor1_stats_list[3], "freshness": neighbor1_stats_list[4], "tx_count": neighbor1_stats_list[5],
                  "ack_count": neighbor1_stats_list[6]}

        all_statistics[mac_index]["l_stats_of_neighbours"].append(stats1)

    next_line_2 = serial_connection.readline()
    neighbor2_stats_all = next_line_2.decode("utf-8").replace("\n", "")
    neighbor2_stats_list = neighbor2_stats_all.split(" ")

    if neighbor2_stats_list[0] != no_addr_addr:
        stats2 = {"addr": neighbor2_stats_list[0], "last_tx_time": neighbor2_stats_list[1], "etx": neighbor2_stats_list[2],
              "rssi": neighbor2_stats_list[3], "freshness": neighbor2_stats_list[4], "tx_count": neighbor2_stats_list[5],
              "ack_count": neighbor2_stats_list[6]}

        all_statistics[mac_index]["l_stats_of_neighbours"].append(stats2)

    print(all_statistics)

def request_all_statistics_from_all_nodes(serial_connection):
    """
    Sends request and parse answer to all mac addresses.
    :param serial_connection: Serial connection for writing request messages and receiving answers
    """
    for mac_address in mac_addresses:
        request_stats(serial_connection, mac_address.replace(".", ""), 1)
        count = 0

        while count < 60:
            x = serial_connection.readline()
            y = x.decode("utf-8").replace("\n", "")
            content = y.split(" ")
            if len(content) == 2 and content[0] == "1":
                parse_answers_all_statistics(serial_connection)
            count += 1