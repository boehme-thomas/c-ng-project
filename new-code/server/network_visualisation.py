import networkx as nx
import matplotlib.pyplot as plt

def create_graph(statistics):
    """
    Creates graph from statistics.
    :param statistics: Statistics from which the graph is created
    :return: Graph
    """

    # creates a list of addresses in l_stats_of_neighbours, which is used to create a tuple (if addr is not broadcast or beacon addr), and with that tuple, creates the adjacency dictionary
    adjacency_dict = {stats_dict["src_addr"]: {addr for addr in [stats["addr"] for stats in stats_dict["l_stats_of_neighbours"]] if addr != "0000.0000.0000.0000" and addr != "ffff.ffff.ffff.ffff"} for stats_dict in statistics if len(stats_dict["l_stats_of_neighbours"]) > 2}
    g = nx.Graph(adjacency_dict)
    return g

def print_graph(graph, coordinator):
    """
    Prints graph
    :param coordinator: The mac address of the coordinator
    :param graph: Graph to be printed
    :return: None
    """
    pos = nx.spring_layout(graph)
    not_coordinator_list = []
    for node in graph.nodes():
        if node == coordinator:
            nx.draw_networkx_nodes(graph, pos, nodelist=[node], node_color="tab:orange", label="coordinator")
        else:
            not_coordinator_list.append(node)

    nx.draw_networkx_nodes(graph, pos, nodelist=not_coordinator_list, node_color="tab:blue", label="standard nodes")
    nx.draw_networkx_edges(graph, pos, edgelist=graph.edges(), alpha=0.5, width=2)
    nx.draw_networkx_labels(graph, pos, font_size=10, horizontalalignment="center")
    plt.title("Topology")
    plt.tight_layout()
    plt.legend(fontsize=9, markerscale=0.4)
    plt.margins(0.4, 0.4)
    plt.show()