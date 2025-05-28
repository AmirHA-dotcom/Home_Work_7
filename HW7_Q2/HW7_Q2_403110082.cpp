#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iomanip>
#include <algorithm>

using namespace std;
regex add_graph_PAT("^NEW_GRAPH (\\S+)\\s*$");
regex add_vertex_PAT(R"(^ADD_VERTEX (\S+) (\S+) (\S+)\s*$)");
regex add_edge_PAT(R"(^ADD_EDGE (\S+) (\S+) (\S+) (\S+)\s*$)");
regex delete_vertex_PAT(R"(^DEL_VERTEX (\S+) (\S+)\s*$)");
regex delete_edge_PAT(R"(^DEL_EDGE (\S+) (\S+) (\S+)\s*$)");
regex edit_vertex_weight_PAT(R"(^EDIT_VERTEX (\S+) (\S+) (\S+)\s*$)");
regex edit_edge_weight_PAT(R"(^EDIT_EDGE (\S+) (\S+) (\S+) (\S+)\s*$)");
regex show_INFO_PAT("^SHOW_GRAPH (\\S+)\\s*$");
regex calculate_distance_PAT(R"(^GRAPH_DISTANCE (\S+) (\S+)\s*$)");
smatch match;
// ---------- EXCEPTION ----------
class ERROR : exception
{
public:
    const char* what() const noexcept override
    {
        return "INVALID COMMAND";
    }
};

// ---------- MODEL ----------
struct Vertex
{
    string ID;
    double weight;
};

struct Edge
{
    double weight;
    Vertex v1;
    Vertex v2;
};

class Graph
{
private:
    string ID;
    vector<Vertex> vertexes;
    vector<Edge> edges;
    int vertex_finder(string V_ID)
    {
        for (int i = 0; i < vertexes.size(); i++)
        {
            if (V_ID == vertexes[i].ID)
                return i;
        }
        throw ERROR();
    }
    int edge_finder(Vertex v1, Vertex v2)
    {
        for (int i = 0; i < edges.size(); i++)
        {
            if (v1.ID == edges[i].v1.ID || v2.ID == edges[i].v2.ID)
                return i;
        }
        throw ERROR();
    }
public:
    Graph(string number) : ID(number) {}
    string get_ID() const{return ID;}
    void add_vertex(string V_ID, double weight)
    {
        Vertex new_vertex{V_ID, weight};
        vertexes.push_back(new_vertex);
    }
    void add_edge(string V_ID_1, string V_ID_2, double weight)
    {
        Vertex V1 = vertexes[vertex_finder(V_ID_1)];
        Vertex V2 = vertexes[vertex_finder(V_ID_2)];
        Edge new_edge{weight, V1, V2};
        edges.push_back(new_edge);
    }
    void delete_vertex(string vertex_ID)
    {
        int vertex_index = vertex_finder(vertex_ID);
        Vertex vertex = vertexes[vertex_index];
        for (auto it = edges.begin(); it != edges.end();)
        {
            if (it->v1.ID == vertex.ID || it->v2.ID == vertex.ID)
                it = edges.erase(it);
            else
                it++;
        }
        vertexes.erase(vertexes.begin() + vertex_index);
    }
    void delete_edge(string V_ID_1, string V_ID_2)
    {
        Vertex V1 = vertexes[vertex_finder(V_ID_1)];
        Vertex V2 = vertexes[vertex_finder(V_ID_2)];
        int edge_index = edge_finder(V1, V2);
        edges.erase(edges.begin() + edge_index);
    }
    void edit_vertex_edge(string V_ID, double weight)
    {
        vertexes[vertex_finder(V_ID)].weight = weight;
    }
    void edit_edge_weight(string V_ID_1, string V_ID_2, double weight)
    {
        Vertex V1 = vertexes[vertex_finder(V_ID_1)];
        Vertex V2 = vertexes[vertex_finder(V_ID_2)];
        edges[edge_finder(V1, V2)].weight = weight;
    }
    void display_INFO()
    {
        cout << ID << " " << vertexes.size() << " " << edges.size() << endl;
        sort(vertexes.begin(), vertexes.end(), [](const Vertex& a, const Vertex& b)
        {
            return a.ID < b.ID;
        });
        sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b)
        {
            if (a.v1.ID != b.v1.ID)
                return a.v1.ID < b.v1.ID;
            return a.v2.ID < b.v2.ID;
        });
        for (const auto& vertex : vertexes)
            cout << ID << " " << vertex.ID << " " << fixed << setprecision(2) << vertex.weight << endl;
        for (const auto& edge : edges)
            cout << ID << " " << edge.v1.ID << " " << edge.v2.ID << " " << fixed << setprecision(2) << edge.weight << endl;
    }
};

// ---------- CONTROLLER ----------
class Controller
{
private:
    vector<Graph*> graphs;
    int graph_finder(string ID)
    {
        for (int i = 0; i < graphs.size(); i++)
        {
            if (ID == graphs[i]->get_ID())
                return i;
        }
        return -1;
    }
public:
    Controller() {}
    void create_graph(string ID)
    {
        if (graph_finder(ID) != -1)
            throw ERROR();
        graphs.push_back(new Graph(ID));
    }
    void add_vertex(string G_ID, string V_ID, double weight)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->add_vertex(V_ID, weight);
    }
    void add_edge(string G_ID, string V_ID_1, string V_ID_2, double weight)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->add_edge(V_ID_1, V_ID_2, weight);
    }
    void delete_vertex(string G_ID, string V_ID)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->delete_vertex(V_ID);
    }
    void delete_edge(string G_ID, string V_ID_1, string V_ID_2)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->delete_edge(V_ID_1, V_ID_2);
    }
    void edit_vertex_weight(string G_ID, string V_ID, double weight)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->edit_vertex_edge(V_ID, weight);
    }
    void edit_edge_weight(string G_ID, string V_ID_1, string V_ID_2, double weight)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->edit_edge_weight(V_ID_1, V_ID_2, weight);
    }
    void show_graph(string G_ID)
    {
        int graph_index = graph_finder(G_ID);
        if (graph_index == -1)
            throw ERROR();
        graphs[graph_index]->display_INFO();
    }
    ~Controller()
    {
        for (auto* g : graphs)
            delete g;
    }
};

// ---------- VIEW ----------
class View
{
private:
    Controller AHA;
public:
    void DO_EVERYTHING(string command)
    {
        if (regex_match(command, match, add_graph_PAT))
        {
            AHA.create_graph(match[1]);
        }
        else if (regex_match(command, match, add_vertex_PAT))
        {
            AHA.add_vertex(match[1], match[2], stod(match[3]));
        }
        else if (regex_match(command, match, add_edge_PAT))
        {
            AHA.add_edge(match[1], match[2], match[3], stod(match[4]));
        }
        else if (regex_match(command, match, edit_vertex_weight_PAT))
        {
            AHA.edit_vertex_weight(match[1], match[2], stod(match[3]));
        }
        else if (regex_match(command, match, edit_edge_weight_PAT))
        {
            AHA.edit_edge_weight(match[1], match[2], match[3], stod(match[4]));
        }
        else if (regex_match(command, match, delete_vertex_PAT))
        {
            AHA.delete_vertex(match[1], match[2]);
        }
        else if (regex_match(command, match, delete_edge_PAT))
        {
            AHA.delete_edge(match[1], match[2], match[3]);
        }
        else if (regex_match(command, match, show_INFO_PAT))
        {
            AHA.show_graph(match[1]);
        }
        else if (regex_match(command, match, calculate_distance_PAT))
        {

        }
        else
            throw ERROR();
    }
};

// ---------- MAIN ----------
int main()
{
    View AHA;
    string command;
    int how_many;
    cin >> how_many;
    getline(cin, command);
    for (int i = 0; i < how_many; i++)
    {
        getline(cin, command);
        try
        {
            AHA.DO_EVERYTHING(command);
        }
        catch (const exception& e)
        {
            cout << e.what() << endl;
        }
    }
}
