import networkx as nx
import serial
import statistics_collection
import network_visualisation

if __name__ == '__main__':
    serial_line = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1, xonxoff=False, rtscts=False, dsrdtr=True)
    # ser.write(b"tsch-send 2 -2 -2 f4ce36daa6f8362b f4ce36d4a43687a8\n")
    # tsch_send(ser, 2, 2, 2, "f4ce36daa6f8362b", "f4ce36d4a43687a8")

    statistics_collection.request_all_statistics_from_all_nodes(serial_line)
    #statistics_collection.request_stats(serial_line, addr="f4ce.36da.a6f8.362b", all_stats=1)

    graph = network_visualisation.create_graph(statistics_collection.all_statistics)

    network_visualisation.print_graph(graph, statistics_collection.coordinator)