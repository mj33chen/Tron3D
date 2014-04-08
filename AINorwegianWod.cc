#include "Player.hh"
#include <vector>
#include <queue>
#include <limits>


using namespace std;


#define PLAYER_NAME                 NorwegianW1d
#define PLAYER_NAME_STRING          "NorwegianW1d"

struct PLAYER_NAME : public Player { 

    static PLAYER_NAME* inst;

    /**
     * Represents a table of distance for reach the vertex (id vertex) of the map 3D.
     * It will be updated in every radar search.
     */
    typedef vector<int> CostTable;

    /**
     *
     *
     */
    typedef vector<bool> Marks;

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
     * The bike of the player
     */
    Bike NWme;

    /**
     * Costs for every vertex in the last radar of the map
     */
    CostTable table_costs;

    /**
     * Represents a queue of the pending vertexs to visit.
     * It will be updated in every radar
     */
    queue<Vertex> pending;

    /**
     *
     *
     */
    Marks mark;


    /**
     *
     *
     */
    bool is_available(const Vertex &v)
    {
        if(v.wall == -1 and v.bike == -1)
        {
            vector<int> neighbours2 = vertex(v.id).neighbours;
            for(int i = 0; i < int(neighbours2.size()); ++i)
            {
                int id = neighbours2[i];
                if(not mark[id])
                {
                    if(vertex(id).bike == NWme.id) return false;
                }
            }
            return true;
        }
        return false;
    }

    /**
     * Detects bikes inside a radius r from the vertex v.
     * @param v     Position of the vertex as the center to start the radar-
     * @param r     Radius to search.
     * @param bikes Empty vector to store bikes or walls found.
     * @return      True if one or more bike or walls has been found,
     *              false otherwise.
     */
    void radar(const Vertex &v, Movement &movement)
    {
        pending = queue<Vertex>();
        mark = Marks(nb_vertices(), false);

        table_costs = CostTable(nb_vertices(), numeric_limits<int>::max());   // distance from the vertex parent 
        vector<int> back(nb_vertices(), -2);

        pending.push(v);
        mark[v.id] = true;  // visited 
        table_costs[v.id] = 0; // distance 0
        back[v.id] = -1;

        int last_position = -1;
        while(not pending.empty())
        {
            //cerr << "entrado" << endl;
            //vector<int> empty_neighbours;  // vertexs availables, empty initially 

            Vertex u = pending.front();
            //cerr << "identificador de vertice padre " << u.id << endl;
            last_position = u.id;
            pending.pop();
            //cerr << "he quitado un vertice de la cola" << endl;

            vector<int> neighbours = vertex(u.id).neighbours;
            for(int i = 0; i < int(neighbours.size()); ++i)
            {
                if(is_available(vertex(neighbours[i])) and not mark[neighbours[i]]) 
                {
                    int id = neighbours[i];
                    mark[id] = true;                         // visited 
                    int new_cost = table_costs[u.id] + 1;    // add distance 1 from the vertex parent 
                    //empty_neighbours.push_back(id);

                    if(new_cost < table_costs[id])
                    {
                        table_costs[id] = new_cost;
                        pending.push(vertex(id));            // push vertex 
                        back[id] = u.id;
                    }
                }
            }

            if(neighbours.size() == 0 and NWme.bonus == Ghost)
            {
                //cerr << "no queda más camino " << endl;
                movement.use_bonus = true;
            }
        }

        int vertex_ = v.id;
        int measure = last_position;

        while(back[measure] != -1)
        {
            vertex_ = measure;
            measure = back[measure]; 
        }
        //pause();
        movement.next_vertex = vertex_;
    }

    bool has_turn()
    {
        return(NWme.alive and round()%2 == 0);
    }


    /**
     * Play method.
     * 
     * This method will be invoked once per each round.
     * You have to read the board and place your actions
     * for this round.
     */
    void play ()
    {

        vector<int> my_bikes = bikes(me());

        if(round() == 0)
        {
            inst = this;
        }

        for(int i = 0; i < (int)my_bikes.size(); ++i)
        {
            NWme = bike(my_bikes[i]);

            // Create an empty movement
            Movement movement(NWme.id);
            if(has_turn())
            {
                if(round() == 0) wander(movement);

                else
                {
                    radar(vertex(NWme.vertex), movement);
                }
                command(movement);
            }
        }
    }


    /**
     * Makes the player wander.
     */
    void wander(Movement &movement)
    {
        vector<int> neighbours = vertex(NWme.vertex).neighbours;
        movement.next_vertex = neighbours[ rand() % (int)neighbours.size() ];
    }

};


PLAYER_NAME* PLAYER_NAME::inst = NULL;

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);


