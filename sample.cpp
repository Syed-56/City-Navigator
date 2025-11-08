// #include <iostream>
// #include <vector>
// #include <list>
// #include <algorithm>
// #include <queue>
// #include <unordered_map>
// #include <climits>
// using namespace std;

// // ---------------- CITY INFO (Linked List Node) ----------------
// struct CityNode {
//     string name;
//     int population;
//     CityNode* next;

//     CityNode(string n, int p) : name(n), population(p), next(nullptr) {}
// };

// // ---------------- GRAPH CLASS ----------------
// class Graph {
//     int V; // number of vertices
//     unordered_map<string, list<pair<string, int>>> adjList; // city -> [(neighbor, distance)]

// public:
//     Graph(int v = 0) { V = v; }

//     void addEdge(string city1, string city2, int distance) {
//         adjList[city1].push_back({city2, distance});
//         adjList[city2].push_back({city1, distance});
//     }

//     void removeEdge(string city1, string city2) {
//         adjList[city1].remove_if([&](auto &p){ return p.first == city2; });
//         adjList[city2].remove_if([&](auto &p){ return p.first == city1; });
//     }

//     void displayConnections() {
//         cout << "\n--- City Connections ---\n";
//         for (auto &pair : adjList) {
//             cout << pair.first << " -> ";
//             for (auto &nbr : pair.second)
//                 cout << "(" << nbr.first << ", " << nbr.second << " km) ";
//             cout << endl;
//         }
//     }

//     // BFS to find shortest path (by edges, not distance)
//     void shortestPath(string start, string end) {
//         unordered_map<string, bool> visited;
//         unordered_map<string, string> parent;
//         queue<string> q;

//         q.push(start);
//         visited[start] = true;
//         parent[start] = "";

//         bool found = false;
//         while (!q.empty()) {
//             string cur = q.front(); q.pop();

//             if (cur == end) { found = true; break; }

//             for (auto &nbr : adjList[cur]) {
//                 if (!visited[nbr.first]) {
//                     visited[nbr.first] = true;
//                     parent[nbr.first] = cur;
//                     q.push(nbr.first);
//                 }
//             }
//         }

//         if (!found) {
//             cout << "No path found between " << start << " and " << end << ".\n";
//             return;
//         }

//         // reconstruct path
//         vector<string> path;
//         for (string at = end; at != ""; at = parent[at])
//             path.push_back(at);
//         reverse(path.begin(), path.end());

//         cout << "\nShortest Path: ";
//         for (int i = 0; i < path.size(); i++) {
//             cout << path[i];
//             if (i != path.size() - 1) cout << " -> ";
//         }
//         cout << endl;
//     }
// };

// // ---------------- CITY INFO LIST CLASS ----------------
// class CityList {
//     CityNode* head;

// public:
//     CityList() : head(nullptr) {}

//     void addCity(string name, int population) {
//         CityNode* newNode = new CityNode(name, population);
//         newNode->next = head;
//         head = newNode;
//     }

//     void removeCity(string name) {
//         if (!head) return;
//         if (head->name == name) {
//             CityNode* temp = head;
//             head = head->next;
//             delete temp;
//             return;
//         }

//         CityNode* cur = head;
//         while (cur->next && cur->next->name != name)
//             cur = cur->next;

//         if (cur->next) {
//             CityNode* temp = cur->next;
//             cur->next = cur->next->next;
//             delete temp;
//         }
//     }

//     void displayCities() {
//         cout << "\n--- City Information ---\n";
//         CityNode* cur = head;
//         while (cur) {
//             cout << "City: " << cur->name << " | Population: " << cur->population << endl;
//             cur = cur->next;
//         }
//     }

//     bool exists(string name) {
//         CityNode* cur = head;
//         while (cur) {
//             if (cur->name == name) return true;
//             cur = cur->next;
//         }
//         return false;
//     }
// };

// // ---------------- MAIN PROGRAM ----------------
// int main() {
//     CityList cityList;
//     Graph graph;
//     vector<string> cityArray; // array of all city names

//     // --- Demo Data ---
//     cityList.addCity("Karachi", 14910000);
//     cityList.addCity("Lahore", 11120000);
//     cityList.addCity("Islamabad", 1095000);
//     cityList.addCity("Peshawar", 1970042);
//     cityList.addCity("Quetta", 1002000);

//     cityArray = {"Karachi", "Lahore", "Islamabad", "Peshawar", "Quetta"};

//     graph.addEdge("Karachi", "Lahore", 1210);
//     graph.addEdge("Lahore", "Islamabad", 375);
//     graph.addEdge("Islamabad", "Peshawar", 160);
//     graph.addEdge("Karachi", "Quetta", 690);
//     graph.addEdge("Quetta", "Peshawar", 820);

//     int choice;
//     string c1, c2;
//     int dist;

//     while (true) {
//         cout << "\n============================";
//         cout << "\n  CITY ROUTE PLANNER MENU";
//         cout << "\n============================";
//         cout << "\n1. Display Cities";
//         cout << "\n2. Display Connections";
//         cout << "\n3. Add City";
//         cout << "\n4. Add Connection";
//         cout << "\n5. Remove City";
//         cout << "\n6. Remove Connection";
//         cout << "\n7. Find Shortest Path";
//         cout << "\n0. Exit";
//         cout << "\nEnter choice: ";
//         cin >> choice;

//         if (choice == 0) break;

//         switch (choice) {
//         case 1:
//             cityList.displayCities();
//             break;

//         case 2:
//             graph.displayConnections();
//             break;

//         case 3: {
//             string name; int pop;
//             cout << "Enter city name: "; cin >> name;
//             cout << "Enter population: "; cin >> pop;
//             cityList.addCity(name, pop);
//             cityArray.push_back(name);
//             cout << "City added successfully.\n";
//             break;
//         }

//         case 4:
//             cout << "Enter first city: "; cin >> c1;
//             cout << "Enter second city: "; cin >> c2;
//             cout << "Enter distance (km): "; cin >> dist;
//             graph.addEdge(c1, c2, dist);
//             cout << "Connection added successfully.\n";
//             break;

//         case 5:
//             cout << "Enter city name to remove: "; cin >> c1;
//             cityList.removeCity(c1);
//             cout << "City removed successfully.\n";
//             break;

//         case 6:
//             cout << "Enter first city: "; cin >> c1;
//             cout << "Enter second city: "; cin >> c2;
//             graph.removeEdge(c1, c2);
//             cout << "Connection removed successfully.\n";
//             break;

//         case 7:
//             cout << "Enter start city: "; cin >> c1;
//             cout << "Enter destination city: "; cin >> c2;
//             graph.shortestPath(c1, c2);
//             break;

//         default:
//             cout << "Invalid choice.\n";
//         }
//     }

//     cout << "\nProgram Ended.\n";
//     return 0;
// }
