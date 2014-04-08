#include "Player.hh"
#include <vector>
#include <queue>
#include <limits>

using namespace std;


#define PLAYER_NAME                 im_feelin_22
#define PLAYER_NAME_STRING          "im_feelin_22"

/**
 * Player struct ! 
 */
struct PLAYER_NAME : public Player { 

    /**
     * Player instance pointer. Useful to find the player
     * instance from the "outside".
     */
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
     *
     *
     *
     */
    typedef vector<int> Back_to_Dec;

    /**
     *
     *
     *
     */
    //typedef vector<bool> Nw_visited;

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
    Back_to_Dec back;

    /**
     *
     *
     */
    Marks mark;

    /**
     *
     *
     */
    //Nw_visited visited;

    /*bool is_ally(int bike_id)
    {
        vector<int> my_bikes = bikes(me()); 
        for(int i = 0; i < int(my_bikes.size()); ++i)
        {
            if(bike_id != NWme.id and bike_id == bike(my_bikes[i]).id) return false;
        }
        return true;
    }*/

    /**
     *
     *
     *
     */
    bool not_obstacle(int id)
    {
        return (not mark[id] and vertex(id).wall == -1 and vertex(id).bike == -1);
    }

    /**
     *
     *
     *
     */
    /*bool be_found(Bonus bonus)
    {

    }*/

    /**
     *
     *
     */
    bool is_available(const Vertex &v)
    {
        vector<int> neighbours2 = vertex(v.id).neighbours;
        for(int i = 0; i < int(neighbours2.size()); ++i)
        {
            int id = neighbours2[i];
            if(not mark[id])
            {
                if(vertex(id).bike != -1) return false;
            }
            //if(round() == 5) pause();
        }
        return true;
    }

    /**
     * Select the vertexs which is not wall and not has bikes presents
     * @param v         vector of the vertex that the NWme presents
     * @param w         Empty vector
     * @return          Empty vector if has none vertex, else a vector contains 
     *                      vertex available 
     */
    void transformer(const vector<int> &neighbours, vector<int> &empty_neighbours)
    {
        vector<int> bikes_neigh;
        vector<int> non_bikes_neigh;
        
        for(int i = 0; i < int(neighbours.size()); ++i)
        {
            int id = neighbours[i];
            if(not_obstacle(id))
            {
                if(is_available(vertex(id)))
                {
                    non_bikes_neigh.push_back(id);
                }
                else bikes_neigh.push_back(id);
            }
        }
        if(non_bikes_neigh.empty()) empty_neighbours = bikes_neigh;
        else empty_neighbours = non_bikes_neigh;
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
        bool detected = false;
        //cerr << "v.bike es " << v.bike << endl;
        //pause();

        //visited = Nw_visited(nb_vertices(), false);
        //visited[v.id] = true;

        pending = queue<Vertex>();
        mark = Marks(nb_vertices(), false);

        table_costs = CostTable(nb_vertices(), numeric_limits<int>::max());   // distance from the vertex parent 
        back = Back_to_Dec(nb_vertices(), -2);

        pending.push(v);
        mark[v.id] = true;  // visited 
        table_costs[v.id] = 0; // distance 0
        back[v.id] = -1;

        int last_position = -1;

        vector<int> empty_neighbours;  // vector of vertexs availables, empty initially 

        while(not pending.empty() and not detected)
        {
            //cerr << "entrado" << endl;

            Vertex u = pending.front();

            if(u.bonus == Ghost) detected = true;
            else if(u.bonus == Turbo) detected = true;
            else if(u.bonus == Points) detected = true;

            /*if(be_found(u.bonus)) detected = true;
            else detected = false;*/

            //cerr << "identificador de vertice padre " << u.id << endl;
            last_position = u.id;
            pending.pop();
            //cerr << "he quitado un vertice de la cola" << endl;

            vector<int> neighbours = vertex(u.id).neighbours;

            transformer(neighbours, empty_neighbours);

            for(int i = 0; i < int(empty_neighbours.size()); ++i)
            {
                //if(is_available(vertex(empty_neighbours[i]))) 
                //{
                    int id = empty_neighbours[i];
                    mark[id] = true;                         // visited 
                    int new_cost = table_costs[u.id] + 1;    // add distance 1 from the vertex parent 
                    //

                    if(new_cost < table_costs[id])
                    {
                        table_costs[id] = new_cost;
                        pending.push(vertex(id));            // push vertex 
                        back[id] = u.id;
                    }
                //}
            }
        }
 
        int vertex_ = v.id;
        int measure = last_position; 

        while(back[measure] != -1)
        {
            vertex_ = measure;
            measure = back[measure]; 
        }

        if(empty_neighbours.empty() and NWme.bonus == Ghost)
        {
            movement.use_bonus = true;
        }

        else if(NWme.bonus == Points or NWme.bonus == Turbo)  
        {
            movement.use_bonus = true; 
        }

        movement.next_vertex = vertex_;
    }

    bool has_turn()
    {
        return(NWme.alive and (round()%2 == 0 or NWme.bonus == Turbo));
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
            //cerr << "NWme.id " << NWme.id << endl;
            // Create an empty movement
            Movement movement(NWme.id);
            if(has_turn())
            {
                //cerr << "Soy el  " << NWme.id << endl;
                radar(vertex(NWme.vertex), movement);

                command(movement);
            }
        }
    }


};


PLAYER_NAME* PLAYER_NAME::inst = NULL;

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);